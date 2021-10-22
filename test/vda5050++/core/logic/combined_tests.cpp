// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/instant_actions_manager.h"
#include "vda5050++/interface_agv/handle.h"

static void testActionStatus(vda5050pp::interface_agv::Handle &handle, std::string id,
                             vda5050pp::ActionStatus status) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();

  THEN(vda5050pp::core::common::logstring("Action ", id, " is ", status)) {
    auto actual_status = state.action_state_by_id[id].actionStatus;
    REQUIRE(actual_status == status);
  }
}

static void testActionStatus(vda5050pp::interface_agv::Handle &handle,
                             std::initializer_list<std::string> ids,
                             vda5050pp::ActionStatus status) {
  for (const auto &id : ids) {
    testActionStatus(handle, id, status);
  }
}

static void testDriving(vda5050pp::interface_agv::Handle &handle, bool required_driving) {
  auto text = required_driving ? "The AGV is driving" : "The AGV is not driving";
  THEN(text) {
    vda5050pp::core::interface_agv::HandleAccessor ha(handle);
    const auto &unsafe = ha.getState().getStateUnsafe();
    const auto &actual_driving = unsafe.state.driving;
    REQUIRE(actual_driving == required_driving);
  }
}

TEST_CASE("core::logic - cancelOrder", "[core][logic][combined]") {
  GIVEN("An initialzed handle with a running order") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;

    vda5050pp::interface_agv::Handle handle({}, std::make_shared<test::TestConnector>(), Handlers{},
                                            logger);

    // Handler behaviour ///////////////////////////////////////////////////////
    test::TestStepBasedNavigationHandler::setAutoFinishOnStop(true);
    test::TestStepBasedNavigationHandler::setAutoFailOnStop(false);
    test::TestStepBasedNavigationHandler::setAutoPause(true);
    test::TestStepBasedNavigationHandler::setAutoResume(true);
    test::TestStepBasedNavigationHandler::setAutoStart(false);
    test::TestActionHandler::setAutoFinishOnStop(true);
    test::TestActionHandler::setAutoFailOnStop(false);
    test::TestActionHandler::setAutoPause(true);
    test::TestActionHandler::setAutoResume(true);
    test::TestActionHandler::setAutoStart(true);
    ////////////////////////////////////////////////////////////////////////////

    // Order ///////////////////////////////////////////////////////////////////
    vda5050pp::Action pick1 = {"pick", "a1", std::nullopt, vda5050pp::BlockingType::HARD, {}};
    vda5050pp::Action drop1 = {"drop", "a2", std::nullopt, vda5050pp::BlockingType::HARD, {}};
    vda5050pp::Action pick2 = {"pick", "a3", std::nullopt, vda5050pp::BlockingType::HARD, {}};
    vda5050pp::Action drop2 = {"drop", "a4", std::nullopt, vda5050pp::BlockingType::HARD, {}};

    vda5050pp::Order order = {
        {},
        "order1",
        0,
        std::nullopt,
        {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {pick1}),
         test::mkNode("n3", 4, true, {drop1, pick2})},
        {test::mkEdge("e1", 1, true, "n1", "n2", {}), test::mkEdge("e2", 3, true, "n2", "n3", {})}};

    vda5050pp::core::interface_agv::HandleAccessor ha(handle);
    ha.getState().setOrder(order);
    ha.getLogic().interpretOrder();
    handle.spinAll();
    ////////////////////////////////////////////////////////////////////////////

    // Test functions //////////////////////////////////////////////////////////
    auto testAction = [&](bool expect, std::string id, vda5050pp::ActionStatus status) {
      std::stringstream ss;
      ss << "Action " << id << " is " << (expect ? "" : "not ") << status;
      THEN(ss.str()) {
        auto state = ha.getState().getStateUnsafe().action_state_by_id[id];
        if (expect) {
          REQUIRE(state.actionStatus == status);
        } else {
          REQUIRE_FALSE(state.actionStatus == status);
        }
      }
    };
    auto testLastNode = [&](bool expect, uint32_t seq) {
      std::stringstream ss;
      ss << "Node " << seq << " was " << (expect ? "" : "not ") << "reached";
      THEN(ss.str()) {
        auto real_seq = ha.getState().getStateUnsafe().state.lastNodeSequenceId;
        if (expect) {
          REQUIRE(seq == real_seq);
        } else {
          REQUIRE_FALSE(seq == real_seq);
        }
      }
    };
    auto testCancel = [&](std::initializer_list<std::string> finished_actions,
                          std::initializer_list<std::string> failed_actions,
                          uint32_t expected_seq) {
      WHEN("The order was canceled") {
        ha.getLogic().abortOrder();
        handle.spinAll();

        for (auto action : finished_actions) {
          testAction(true, action, vda5050pp::ActionStatus::FINISHED);
        }
        for (auto action : failed_actions) {
          testAction(true, action, vda5050pp::ActionStatus::FAILED);
        }
        testLastNode(true, expected_seq);
      }
    };
    ////////////////////////////////////////////////////////////////////////////

    WHEN("Driving to n2 finished") {
      test::navigate_to_node_handler_by_seq.at(2).get().doStarted();
      test::navigate_to_node_handler_by_seq.at(2).get().doFinished();
      handle.spinAll();
      testAction(true, "a1", vda5050pp::ActionStatus::RUNNING);
      testLastNode(true, 2);
      testCancel({"a1"}, {"a2", "a3"}, 2);

      WHEN("A1 finishes") {
        test::test_action_handler_by_id.at("a1").get().doFinished();
        handle.spinAll();
        testLastNode(true, 2);
        // Order is canceled upon finishing running tasks, so a2 and a3 are still waiting
        testAction(true, "a2", vda5050pp::ActionStatus::WAITING);
        testAction(true, "a3", vda5050pp::ActionStatus::WAITING);

        WHEN("Driving to n3 finishes") {
          test::navigate_to_node_handler_by_seq.at(4).get().doStarted();
          test::navigate_to_node_handler_by_seq.at(4).get().doFinished();
          handle.spinAll();
          testLastNode(true, 4);
          testCancel({"a1", "a2"}, {"a3"}, 4);

          WHEN("A2 finishes") {
            test::test_action_handler_by_id.at("a2").get().doFinished();
            handle.spinAll();
            testLastNode(true, 4);
            testAction(true, "a2", vda5050pp::ActionStatus::FINISHED);
            testCancel({"a1", "a2", "a3"}, {}, 4);

            WHEN("A3 finishes") {
              test::test_action_handler_by_id.at("a3").get().doFinished();
              handle.spinAll();
              testLastNode(true, 4);
              testAction(true, "a3", vda5050pp::ActionStatus::FINISHED);
              testCancel({"a1", "a2", "a3"}, {}, 4);
            }
          }
        }
      }
    }
  }
}

TEST_CASE("step based navigation - interrupted navigation", "[core][logic][navigation][combined]") {
  GIVEN("An initialized Handle and an order") {
    test::navigate_to_node_handler_by_seq.clear();
    test::test_action_handler_by_id.clear();

    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                       test::TestActionHandler, test::TestPauseResumeHandler>
        handlers;
    vda5050pp::interface_agv::Handle handle({}, std::make_shared<test::TestConnector>(), handlers,
                                            logger);
    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);
    auto &state = handle_accessor.getState();
    auto &logic = handle_accessor.getLogic();

    test::TestActionHandler::setAutoStart(true);
    test::TestActionHandler::setAutoPause(true);
    test::TestActionHandler::setAutoResume(true);
    test::TestActionHandler::setAutoFailOnStop(true);
    test::TestActionHandler::setAutoFinishOnStop(false);
    test::TestStepBasedNavigationHandler::setAutoStart(true);
    test::TestStepBasedNavigationHandler::setAutoPause(true);
    test::TestStepBasedNavigationHandler::setAutoResume(true);
    test::TestStepBasedNavigationHandler::setAutoFailOnStop(false);

    vda5050pp::Action action1 = {"s1", "a1", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action2 = {"s2", "a2", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
    vda5050pp::Action action3 = {"s3", "a3", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
    vda5050pp::Action action4 = {"s4", "a4", std::nullopt, vda5050pp::BlockingType::NONE, {}};

    vda5050pp::Order order = {
        {},
        "order1",
        0,
        std::nullopt,
        {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {action1}),
         test::mkNode("n3", 4, true, {action2, action3, action4}), test::mkNode("n4", 6, true, {})},
        {test::mkEdge("e1", 1, true, "n1", "n2", {}), test::mkEdge("e2", 3, true, "n2", "n3", {}),
         test::mkEdge("e3", 5, true, "n3", "n4", {})}};

    state.setOrder(order);
    logic.interpretOrder();

    WHEN("The library spins") {
      handle.spinAll();
      THEN("A step based navigation handler with n1->n2 is created") {
        auto &handler = test::navigate_to_node_handler_by_seq.at(2).get();
        REQUIRE(handler.timesStartCalled() == 1);
        REQUIRE(handler.getViaEdge()->startNodeId == "n1");
        REQUIRE(handler.getViaEdge()->endNodeId == "n2");
        REQUIRE(handler.getGoalNode().nodeId == "n2");
        testDriving(handle, true);
      }

      WHEN("N2 was not yet reached") {
        auto &handler = test::navigate_to_node_handler_by_seq.at(2).get();
        handle.spinAll();
        testDriving(handle, true);
        testActionStatus(handle, {"a1", "a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);
        WHEN("N2 is reached") {
          handler.doFinished();
          handle.spinAll();
          THEN("A step based navigation handler with n2->n3 is created") {
            handler = test::navigate_to_node_handler_by_seq.at(4).get();
            REQUIRE(handler.getViaEdge()->startNodeId == "n2");
            REQUIRE(handler.getViaEdge()->endNodeId == "n3");
            REQUIRE(handler.getGoalNode().nodeId == "n3");
          }
          testDriving(handle, true);
          testActionStatus(handle, {"a1"}, vda5050pp::ActionStatus::RUNNING);
          testActionStatus(handle, {"a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);
          WHEN("N3 is reached") {
            handler = test::navigate_to_node_handler_by_seq.at(4).get();
            handler.doFinished();
            handle.spinAll();
            testDriving(handle, false);
            testActionStatus(handle, {"a1", "a2", "a3", "a4"}, vda5050pp::ActionStatus::RUNNING);
            WHEN("A2 finishes") {
              test::test_action_handler_by_id.at("a2").get().doFinished();
              handle.spinAll();
              testDriving(handle, false);
              testActionStatus(handle, {"a1", "a3", "a4"}, vda5050pp::ActionStatus::RUNNING);
              testActionStatus(handle, {"a2"}, vda5050pp::ActionStatus::FINISHED);
              WHEN("A3 finishes") {
                test::test_action_handler_by_id.at("a3").get().doFinished();
                handle.spinAll();
                testDriving(handle, true);
                testActionStatus(handle, {"a1", "a4"}, vda5050pp::ActionStatus::RUNNING);
                testActionStatus(handle, {"a2", "a3"}, vda5050pp::ActionStatus::FINISHED);
                handler = test::navigate_to_node_handler_by_seq.at(6).get();
                THEN("The next step based nav handler  contains n3-n4") {
                  REQUIRE(handler.getGoalNode().nodeId == "n4");
                  REQUIRE(handler.getViaEdge()->startNodeId == "n3");
                  REQUIRE(handler.getViaEdge()->endNodeId == "n4");
                }
                WHEN("N4 was reached") {
                  handler.doFinished();
                  handle.spinAll();
                  testDriving(handle, false);
                }
              }
            }
          }
        }
      }
    }
  }
}

TEST_CASE("step based navigation - action interception", "[core][logic][navigation][combined]") {
  // Base
  test::navigate_to_node_handler_by_seq.clear();
  test::test_action_handler_by_id.clear();
  auto logger = std::make_shared<test::ConsoleLogger>();
  logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
  using Handlers =
      vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                         test::TestActionHandler, test::TestPauseResumeHandler>;

  vda5050pp::interface_agv::Handle handle({}, std::make_shared<test::TestConnector>(), Handlers{},
                                          logger);
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState();

  vda5050pp::core::logic::NetManager net_manager(handle);

  // Handler behaviour
  test::TestActionHandler::setAutoFailOnStop(true);
  test::TestActionHandler::setAutoStart(true);
  test::TestActionHandler::setAutoPause(true);
  test::TestActionHandler::setAutoResume(true);
  test::TestStepBasedNavigationHandler::setAutoFailOnStop(true);
  test::TestStepBasedNavigationHandler::setAutoStart(true);
  test::TestStepBasedNavigationHandler::setAutoPause(true);
  test::TestStepBasedNavigationHandler::setAutoResume(true);

  // Intercept actions
  vda5050pp::Action instant_action_none{
      "Test", "I1", std::nullopt, vda5050pp::BlockingType::NONE, {}};
  vda5050pp::Action instant_action_soft{
      "Test", "I2", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
  vda5050pp::Action instant_action_hard{
      "Test", "I3", std::nullopt, vda5050pp::BlockingType::HARD, {}};
  vda5050pp::Action instant_action_hard2{
      "Test", "I4", std::nullopt, vda5050pp::BlockingType::HARD, {}};

  // Order
  std::vector<vda5050pp::Action> actions1 = {
      {"Test", "A1", std::nullopt, vda5050pp::BlockingType::SOFT, {}},
      {"Test", "A2", std::nullopt, vda5050pp::BlockingType::NONE, {}},
      {"Test", "A3", std::nullopt, vda5050pp::BlockingType::HARD, {}},
      {"Test", "A4", std::nullopt, vda5050pp::BlockingType::NONE, {}},
  };
  std::vector<vda5050pp::Action> actions2 = {
      {"Test", "A5", std::nullopt, vda5050pp::BlockingType::NONE, {}},
  };
  vda5050pp::Order order{{},
                         "testOrder",
                         0,
                         std::nullopt,
                         {test::mkNode("N1", 0, true, actions1), test::mkNode("N2", 2, true, {})},
                         {test::mkEdge("E1", 1, true, "N1", "N2", actions2)}};
  state.setOrder(order);
  net_manager.interpret();

  handle.spinAll();
  handle.spinAll();

  WHEN("A Soft blocking action A1 and a None blocking action A2 is running") {
    handle.spinAll();
    testActionStatus(handle, {"A1", "A2"}, vda5050pp::ActionStatus::RUNNING);
    WHEN("A1 and A2 are intercepted by a HARD blocking instant action") {
      state.insertInstantActions({{}, {instant_action_hard}});
      net_manager.interceptWithAction(instant_action_hard);
      handle.spinAll();
      testActionStatus(handle, {"A1", "A2"}, vda5050pp::ActionStatus::FAILED);
      testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::WAITING);
      testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);

      WHEN("The HARD blocking instant action I3 finishes") {
        test::test_action_handler_by_id.at("I3").get().doFinished();
        handle.spinAll();
        testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::FINISHED);

        WHEN("HARD blocking Action A3 is intercepted by a HARD blocking instant action") {
          state.insertInstantActions({{}, {instant_action_hard}});
          net_manager.interceptWithAction(instant_action_hard);
          handle.spinAll();
          testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::FAILED);
          testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::WAITING);
          testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);
          WHEN("The HARD blocking instant action I3 finishes") {
            test::test_action_handler_by_id.at("I3").get().doFinished();
            handle.spinAll();
            testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::RUNNING);
            testActionStatus(handle, {"A5"}, vda5050pp::ActionStatus::RUNNING);
            testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::FINISHED);
            testDriving(handle, true);

            WHEN("NONE blocking Action A4,A5 is intercepted by a HARD blocking instant action") {
              state.insertInstantActions({{}, {instant_action_hard}});
              net_manager.interceptWithAction(instant_action_hard);
              handle.spinAll();
              testActionStatus(handle, {"A4", "A5"}, vda5050pp::ActionStatus::FAILED);
              testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);
              testDriving(handle, false);
            }
            WHEN("NONE blocking Action A4,A5 is intercepted by a SOFT blocking instant action") {
              state.insertInstantActions({{}, {instant_action_soft}});
              net_manager.interceptWithAction(instant_action_soft);
              handle.spinAll();
              testActionStatus(handle, {"A4", "A5", "I2"}, vda5050pp::ActionStatus::RUNNING);
              testDriving(handle, false);
            }
            WHEN("NONE blocking Action A4,A5 is intercepted by a NONE blocking instant action") {
              state.insertInstantActions({{}, {instant_action_none}});
              net_manager.interceptWithAction(instant_action_none);
              handle.spinAll();
              testActionStatus(handle, {"A4", "A5", "I1"}, vda5050pp::ActionStatus::RUNNING);
              testDriving(handle, true);
            }
          }
        }
        WHEN("HARD blocking Action A3 is intercepted by a SOFT blocking instant action") {
          state.insertInstantActions({{}, {instant_action_soft}});
          net_manager.interceptWithAction(instant_action_soft);
          handle.spinAll();
          testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::FAILED);
          testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::WAITING);
          testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::RUNNING);
          WHEN("The SOFT blocking instant action I2 finishes") {
            test::test_action_handler_by_id.at("I2").get().doFinished();
            handle.spinAll();
            testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::RUNNING);
            testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::FINISHED);
          }
        }
        WHEN("HARD blocking Action A3 is intercepted by a NONE blocking instant action") {
          state.insertInstantActions({{}, {instant_action_none}});
          net_manager.interceptWithAction(instant_action_none);
          handle.spinAll();
          testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::FAILED);
          testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::WAITING);
          testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::RUNNING);
          WHEN("The NONE blocking instant action I1 finishes") {
            test::test_action_handler_by_id.at("I1").get().doFinished();
            handle.spinAll();
            testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::RUNNING);
            testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::FINISHED);
          }
        }
      }
    }
    WHEN("A1 and A2 are intercepted by a SOFT blocking instant action") {
      state.insertInstantActions({{}, {instant_action_soft}});
      net_manager.interceptWithAction(instant_action_soft);
      handle.spinAll();
      testActionStatus(handle, {"A1", "A2", "I2"}, vda5050pp::ActionStatus::RUNNING);
      testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::WAITING);
      WHEN("The SOFT blocking instant action I2 finishes") {
        test::test_action_handler_by_id.at("I2").get().doFinished();
        handle.spinAll();
        testActionStatus(handle, {"A1", "A2"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::FINISHED);
        testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::WAITING);
        WHEN("A1 A2 finish") {
          test::test_action_handler_by_id.at("A1").get().doFinished();
          test::test_action_handler_by_id.at("A2").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"A1", "A2", "I2"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::RUNNING);
        }
      }
    }
    WHEN("A1 and A2 are intercepted by a NONE blocking instant action") {
      state.insertInstantActions({{}, {instant_action_none}});
      net_manager.interceptWithAction(instant_action_none);
      handle.spinAll();
      testActionStatus(handle, {"A1", "A2", "I1"}, vda5050pp::ActionStatus::RUNNING);
      testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::WAITING);
      WHEN("The NONE blocking instant action I1 finishes") {
        test::test_action_handler_by_id.at("I1").get().doFinished();
        handle.spinAll();
        testActionStatus(handle, {"A1", "A2"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::FINISHED);
        testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::WAITING);
        WHEN("A1 A2 finish") {
          test::test_action_handler_by_id.at("A1").get().doFinished();
          test::test_action_handler_by_id.at("A2").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"A1", "A2", "I1"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A3"}, vda5050pp::ActionStatus::RUNNING);
        }
      }
    }
  }
}