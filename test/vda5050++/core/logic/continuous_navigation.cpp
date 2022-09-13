// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include <catch2/catch.hpp>
#include <set>

#include "test/console_logger.h"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_continuous_navigation_handler.h"
#include "test/test_pause_resume_handler.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/interface_agv/continuous_navigation_handler.h"
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

TEST_CASE("continuous navigation - two node order") {
  GIVEN("a handle and an order") {
    test::continuous_navigation_handler_by_seq.clear();
    test::test_action_handler_by_id.clear();

    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
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
    test::TestContinuousNavigationHandler::setAutoStart(true);
    test::TestContinuousNavigationHandler::setAutoPause(true);
    test::TestContinuousNavigationHandler::setAutoResume(true);
    test::TestContinuousNavigationHandler::setAutoFailOnStop(false);

    vda5050pp::Action action1 = {"s1", "a1", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action2 = {"s2", "a2", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action3 = {"s3", "a3", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action4 = {"s4", "a4", std::nullopt, vda5050pp::BlockingType::NONE, {}};

    std::vector<vda5050pp::Action> actions_n1 = {};
    std::vector<vda5050pp::Action> actions_e1 = {};
    std::vector<vda5050pp::Action> actions_n2 = {};
    std::vector<vda5050pp::Action> actions_e2 = {action3};
    std::vector<vda5050pp::Action> actions_n3 = {action4};

    vda5050pp::Order order = {
        {},
        "order1",
        0,
        std::nullopt,
        {test::mkNode("n1", 0, true, actions_n1), test::mkNode("n2", 2, true, actions_n2)},
        {test::mkEdge("e1", 1, true, "n1", "n2", actions_e1)}};

    state.setOrder(order);
    logic.interpretOrder();

    WHEN("The library spins") {
      handle.spinAll();

      THEN("Continuous driving is initialized knows the whole graph") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        std::set<std::string> n_require = {"n2"};
        std::set<std::string> n_has;
        std::set<std::string> e_require = {"e1"};
        std::set<std::string> e_has;
        REQUIRE(handler.timesStartCalled() == 1);
        for (const auto &node : handler.doGetBaseNodes()) {
          n_has.insert(node.nodeId);
        }
        for (const auto &edge : handler.doGetBaseEdges()) {
          e_has.insert(edge.edgeId);
        }
        REQUIRE(n_require == n_has);
        REQUIRE(e_require == e_has);
        testDriving(handle, true);
      }

      WHEN("The AGV has not reached n2 yet") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        handle.spinAll();
        testDriving(handle, true);
        THEN("Start wasn't called another time") { REQUIRE(handler.timesStartCalled() == 1); }
        WHEN("N2 is reached") {
          handler.doNodeReached(2);
          handle.spinAll();
          testDriving(handle, false);
        }
      }
    }
  }
}

TEST_CASE("continuous navigation - uninterrupted navigation",
          "[core][logic][navigation][combined]") {
  GIVEN("An initialized Handle and an order") {
    test::continuous_navigation_handler_by_seq.clear();
    test::test_action_handler_by_id.clear();

    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
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
    test::TestContinuousNavigationHandler::setAutoStart(true);
    test::TestContinuousNavigationHandler::setAutoPause(true);
    test::TestContinuousNavigationHandler::setAutoResume(true);
    test::TestContinuousNavigationHandler::setAutoFailOnStop(false);

    vda5050pp::Action action1 = {"s1", "a1", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action2 = {"s2", "a2", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action3 = {"s3", "a3", std::nullopt, vda5050pp::BlockingType::NONE, {}};
    vda5050pp::Action action4 = {"s4", "a4", std::nullopt, vda5050pp::BlockingType::NONE, {}};

    std::vector<vda5050pp::Action> actions_n1 = {};
    std::vector<vda5050pp::Action> actions_e1 = {};
    std::vector<vda5050pp::Action> actions_n2 = {action1, action2};
    std::vector<vda5050pp::Action> actions_e2 = {action3};
    std::vector<vda5050pp::Action> actions_n3 = {action4};

    vda5050pp::Order order = {
        {},
        "order1",
        0,
        std::nullopt,
        {test::mkNode("n1", 0, true, actions_n1), test::mkNode("n2", 2, true, actions_n2),
         test::mkNode("n3", 4, true, actions_n3)},
        {test::mkEdge("e1", 1, true, "n1", "n2", actions_e1),
         test::mkEdge("e2", 3, true, "n2", "n3", actions_e2)}};

    state.setOrder(order);
    logic.interpretOrder();

    WHEN("The library spins") {
      handle.spinAll();

      testActionStatus(handle, {"a1", "a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);

      THEN("Continuous driving is initialized knows the whole graph") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        std::set<std::string> n_require = {"n2", "n3"};
        std::set<std::string> n_has;
        std::set<std::string> e_require = {"e1", "e2"};
        std::set<std::string> e_has;
        REQUIRE(handler.timesStartCalled() == 1);
        for (const auto &node : handler.doGetBaseNodes()) {
          n_has.insert(node.nodeId);
        }
        for (const auto &edge : handler.doGetBaseEdges()) {
          e_has.insert(edge.edgeId);
        }
        REQUIRE(n_require == n_has);
        REQUIRE(e_require == e_has);
        testDriving(handle, true);
      }

      WHEN("The AGV has not reached n2 yet") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        handle.spinAll();
        testActionStatus(handle, {"a1", "a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, true);
        THEN("Start wasn't called another time") { REQUIRE(handler.timesStartCalled() == 1); }
        WHEN("N2 is reached") {
          handler.doNodeReached(2);
          handle.spinAll();
          testDriving(handle, true);
          testActionStatus(handle, {"a1", "a2", "a3"}, vda5050pp::ActionStatus::RUNNING);
          testActionStatus(handle, {"a4"}, vda5050pp::ActionStatus::WAITING);

          WHEN("N3 is reached") {
            handler.doNodeReached(4);
            handle.spinAll();
            testDriving(handle, false);
            testActionStatus(handle, {"a1", "a2", "a4"}, vda5050pp::ActionStatus::RUNNING);
            testActionStatus(handle, {"a3"}, vda5050pp::ActionStatus::FAILED);
          }
        }
      }
    }
  }
}

TEST_CASE("continuous navigation - interrupted navigation", "[core][logic][navigation][combined]") {
  GIVEN("An initialized Handle and an order") {
    test::continuous_navigation_handler_by_seq.clear();
    test::test_action_handler_by_id.clear();

    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
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
    test::TestContinuousNavigationHandler::setAutoStart(true);
    test::TestContinuousNavigationHandler::setAutoPause(true);
    test::TestContinuousNavigationHandler::setAutoResume(true);
    test::TestContinuousNavigationHandler::setAutoFailOnStop(false);

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
      THEN("A continuous node handler with n2-n3 is created") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        std::set<std::string> require = {"n2", "n2", "n3"};
        std::set<std::string> has;
        REQUIRE(handler.timesStartCalled() == 1);
        for (const auto &node : handler.doGetBaseNodes()) {
          has.insert(node.nodeId);
        }
        REQUIRE(require == has);
        testDriving(handle, true);
      }

      WHEN("N2 was not yet reached") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        handle.spinAll();
        testDriving(handle, true);
        testActionStatus(handle, {"a1", "a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);
        WHEN("N2 is reached") {
          handler.doNodeReached(2);
          handle.spinAll();
          testDriving(handle, true);
          testActionStatus(handle, {"a1"}, vda5050pp::ActionStatus::RUNNING);
          testActionStatus(handle, {"a2", "a3", "a4"}, vda5050pp::ActionStatus::WAITING);
          WHEN("N3 is reached") {
            handler.doNodeReached(4);
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
                auto &handler2 = test::continuous_navigation_handler_by_seq.at(6).get();
                THEN("Second continuous hander was started") {
                  REQUIRE(handler2.timesStartCalled() == 1);
                  THEN("The second cnav handler only contains n4") {
                    REQUIRE(handler2.doGetBaseNodes().size() == 1);
                    REQUIRE(handler2.doGetBaseNodes().front().nodeId == "n4");
                  }
                }
                WHEN("N4 was reached") {
                  handler2.doNodeReached(6);
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

TEST_CASE("continuous navigation - base increase navigation",
          "[core][logic][navigation][combined]") {
  GIVEN("An initialized Handle and an order") {
    test::continuous_navigation_handler_by_seq.clear();
    test::test_action_handler_by_id.clear();

    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
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
    test::TestContinuousNavigationHandler::setAutoStart(true);
    test::TestContinuousNavigationHandler::setAutoPause(true);
    test::TestContinuousNavigationHandler::setAutoResume(true);
    test::TestContinuousNavigationHandler::setAutoFailOnStop(false);

    vda5050pp::Order order_1 = {{},
                                "order1",
                                0,
                                std::nullopt,
                                {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {})},
                                {test::mkEdge("e1", 1, true, "n1", "n2", {})}};
    vda5050pp::Order order_2 = {
        {},
        "order1",
        1,
        std::nullopt,
        {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {}),
         test::mkNode("n4", 6, true, {})},
        {test::mkEdge("e2", 3, true, "n2", "n3", {}), test::mkEdge("e3", 5, true, "n3", "n4", {})}};

    state.setOrder(order_1);
    logic.interpretOrder();

    WHEN("The library spins") {
      handle.spinAll();
      THEN("A continuous node handler with n2 is created") {
        auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
        std::set<std::string> n_require = {"n2"};
        std::set<std::string> n_has;
        std::set<std::string> e_require = {"e1"};
        std::set<std::string> e_has;
        REQUIRE(handler.timesStartCalled() == 1);
        for (const auto &node : handler.doGetBaseNodes()) {
          n_has.insert(node.nodeId);
        }
        for (const auto &edge : handler.doGetBaseEdges()) {
          e_has.insert(edge.edgeId);
        }
        REQUIRE(n_require == n_has);
        REQUIRE(e_require == e_has);
        testDriving(handle, true);
      }
    }

    WHEN("N2 is reached before the order was extended") {
      handle.spinAll();
      auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
      handler.doNodeReached(2);
      handle.spinAll();
      testDriving(handle, false);
    }
    WHEN("N2 is reached after the order was extended") {
      handle.spinAll();
      auto &handler = test::continuous_navigation_handler_by_seq.at(2).get();
      handle_accessor.getState().appendOrder(order_2);
      handle_accessor.getLogic().interpretOrder();
      handler.doNodeReached(2);
      handle.spinAll();
      testDriving(handle, true);

      THEN("The base contains e2-n4") {
        std::set<std::string> n_require = {"n3", "n4"};
        std::set<std::string> n_has;
        std::set<std::string> e_require = {"e2", "e3"};
        std::set<std::string> e_has;
        REQUIRE(handler.timesStartCalled() == 1);
        for (const auto &node : handler.doGetBaseNodes()) {
          n_has.insert(node.nodeId);
        }
        for (const auto &edge : handler.doGetBaseEdges()) {
          e_has.insert(edge.edgeId);
        }
        REQUIRE(n_require == n_has);
        REQUIRE(e_require == e_has);
      }
      WHEN("N4 is reached") {
        handler.doNodeReached(4);
        handler.doNodeReached(6);
        handle.spinAll();
        testDriving(handle, false);
      }
    }
  }
}

TEST_CASE("continuous navigation - action interception", "[core][logic][navigation][combined]") {
  // Base
  test::continuous_navigation_handler_by_seq.clear();
  test::test_action_handler_by_id.clear();
  auto logger = std::make_shared<test::ConsoleLogger>();
  logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
  using Handlers =
      vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
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
  test::TestContinuousNavigationHandler::setAutoFailOnStop(true);
  test::TestContinuousNavigationHandler::setAutoStart(true);
  test::TestContinuousNavigationHandler::setAutoPause(true);
  test::TestContinuousNavigationHandler::setAutoResume(true);

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