// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/net_manager.h"

#include <catch2/catch.hpp>
#include <functional>
#include <map>
#include <utility>

#include "test/console_logger.h"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/Order.h"

void testActionStatus(vda5050pp::interface_agv::Handle &handle, std::string id,
                      vda5050pp::ActionStatus status) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();

  THEN(vda5050pp::core::common::logstring("Action ", id, " is ", status)) {
    auto actual_status = state.action_state_by_id[id].actionStatus;
    REQUIRE(actual_status == status);
  }
}

void testActionStatus(vda5050pp::interface_agv::Handle &handle,
                      std::initializer_list<std::string> ids, vda5050pp::ActionStatus status) {
  for (const auto &id : ids) {
    testActionStatus(handle, id, status);
  }
}

void testDriving(vda5050pp::interface_agv::Handle &handle, bool driving) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();
  THEN((driving ? "AGV is driving" : "AGV is not driving")) {
    REQUIRE(state.state.driving == driving);
  }
}

TEST_CASE("core::logic::NetManager - simple order interpret", "[core][logic]") {
  GIVEN("A net with one node and a handle which has one order") {
    // Base
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;

    vda5050pp::interface_agv::Handle handle({}, std::make_shared<test::TestConnector>(), Handlers{},
                                            logger);
    auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState();

    // Handler behaviour
    test::TestActionHandler::setAutoFailOnStop(true);
    test::TestActionHandler::setAutoPause(false);
    test::TestActionHandler::setAutoResume(false);
    test::TestActionHandler::setAutoStart(false);
    test::TestStepBasedNavigationHandler::setAutoFailOnStop(true);
    test::TestStepBasedNavigationHandler::setAutoPause(false);
    test::TestStepBasedNavigationHandler::setAutoResume(false);
    test::TestStepBasedNavigationHandler::setAutoStart(false);

    // Order
    std::vector<vda5050pp::Action> actions1 = {
        {"Test", "A1", std::nullopt, vda5050pp::BlockingType::SOFT, {}},
        {"Test", "A2", std::nullopt, vda5050pp::BlockingType::NONE, {}},
        {"Test", "A3", std::nullopt, vda5050pp::BlockingType::SOFT, {}},
        {"Test", "A4", std::nullopt, vda5050pp::BlockingType::HARD, {}},
        {"Test", "A5", std::nullopt, vda5050pp::BlockingType::NONE, {}},
    };
    std::vector<vda5050pp::Action> actions2 = {
        {"Test", "A6", std::nullopt, vda5050pp::BlockingType::NONE, {}},
        {"Test", "A7", std::nullopt, vda5050pp::BlockingType::SOFT, {}},
        {"Test", "A8", std::nullopt, vda5050pp::BlockingType::HARD, {}},
        {"Test", "A9", std::nullopt, vda5050pp::BlockingType::NONE, {}},
    };
    vda5050pp::Order order{{},
                           "testOrder",
                           0,
                           std::nullopt,
                           {test::mkNode("N1", 0, true, actions1), test::mkNode("N2", 2, true, {})},
                           {test::mkEdge("E1", 1, true, "N1", "N2", actions2)}};
    state.appendOrder(order);

    // Manager
    vda5050pp::core::logic::NetManager net_mgr(handle);

    THEN("Initially the graph_seq counters indicate need for interpretation") {
      auto &unsafe_state = state.getStateUnsafe();

      REQUIRE(unsafe_state.graph_base_seq_id == 2);
      REQUIRE(unsafe_state.graph_next_interpreted_seq_id_ == 0);
    }

    WHEN("The Order is interpreted") {
      net_mgr.interpret();

      net_mgr.tick();
      handle.spinAll();

      testActionStatus(handle, {"A1", "A2", "A3"}, vda5050pp::ActionStatus::INITIALIZING);
      testActionStatus(handle, {"A4", "A5", "A6", "A7", "A8", "A9"},
                       vda5050pp::ActionStatus::WAITING);

      WHEN("A1, A2 and A3 start") {
        test::test_action_handler_by_id.at("A1").get().doStarted();
        test::test_action_handler_by_id.at("A2").get().doStarted();
        test::test_action_handler_by_id.at("A3").get().doStarted();
        handle.spinAll();
        testActionStatus(handle, {"A1", "A2", "A3"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A4", "A5", "A6", "A7", "A8", "A9"},
                         vda5050pp::ActionStatus::WAITING);

        WHEN("A1, A2, A3 finish") {
          // TODO: FInish a2 later
          test::test_action_handler_by_id.at("A1").get().doFinished();
          test::test_action_handler_by_id.at("A2").get().doFinished();
          test::test_action_handler_by_id.at("A3").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"A1", "A2", "A3"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A4"}, vda5050pp::ActionStatus::INITIALIZING);
          testActionStatus(handle, {"A5", "A6", "A7", "A8", "A9"},
                           vda5050pp::ActionStatus::WAITING);

          WHEN("A4 finishes") {
            test::test_action_handler_by_id.at("A4").get().doStarted();
            test::test_action_handler_by_id.at("A4").get().doFinished();
            handle.spinAll();
            testActionStatus(handle, {"A1", "A2", "A3", "A4"}, vda5050pp::ActionStatus::FINISHED);
            testActionStatus(handle, {"A5", "A6", "A7"}, vda5050pp::ActionStatus::INITIALIZING);
            testActionStatus(handle, {"A8", "A9"}, vda5050pp::ActionStatus::WAITING);

            WHEN("A6, A7 finish") {
              test::test_action_handler_by_id.at("A6").get().doStarted();
              test::test_action_handler_by_id.at("A6").get().doFinished();
              test::test_action_handler_by_id.at("A7").get().doStarted();
              test::test_action_handler_by_id.at("A7").get().doFinished();
              handle.spinAll();
              testActionStatus(handle, {"A1", "A2", "A3", "A4", "A6", "A7"},
                               vda5050pp::ActionStatus::FINISHED);
              testActionStatus(handle, {"A5", "A8"}, vda5050pp::ActionStatus::INITIALIZING);
              testActionStatus(handle, {"A9"}, vda5050pp::ActionStatus::WAITING);

              THEN("Driving to seq 2 did not start") {
                REQUIRE_THROWS(test::navigate_to_node_handler_by_seq.at(2));
              }

              WHEN("A8 finishes") {
                test::test_action_handler_by_id.at("A8").get().doStarted();
                test::test_action_handler_by_id.at("A8").get().doFinished();
                handle.spinAll();
                testActionStatus(handle, {"A1", "A2", "A3", "A4", "A6", "A7", "A8"},
                                 vda5050pp::ActionStatus::FINISHED);
                testActionStatus(handle, {"A5", "A9"}, vda5050pp::ActionStatus::INITIALIZING);

                WHEN("Driving Finishes") {
                  test::TestStepBasedNavigationHandler &nh =
                      test::navigate_to_node_handler_by_seq.at(2);
                  nh.doStarted();
                  nh.doFinished();
                  handle.spinAll();

                  testActionStatus(handle, {"A1", "A2", "A3", "A4", "A6", "A7", "A8"},
                                   vda5050pp::ActionStatus::FINISHED);
                  testActionStatus(handle, {"A5"}, vda5050pp::ActionStatus::INITIALIZING);
                  testActionStatus(handle, {"A9"}, vda5050pp::ActionStatus::FAILED);
                }

                WHEN("A9 finished") {
                  test::test_action_handler_by_id.at("A9").get().doStarted();
                  test::test_action_handler_by_id.at("A9").get().doFinished();
                  handle.spinAll();
                  testActionStatus(handle, {"A1", "A2", "A3", "A4", "A6", "A7", "A8", "A9"},
                                   vda5050pp::ActionStatus::FINISHED);
                  testActionStatus(handle, {"A5"}, vda5050pp::ActionStatus::INITIALIZING);
                }
              }
            }
          }
        }
      }
    }
  }
}

TEST_CASE("core::logic::NetManager - action interception", "[core][logic]") {
  GIVEN("An initialized Handle and a NetManager with a simple order") {
    // Base
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
}

TEST_CASE("core::logic::NetManager - action interception edge cases", "[core][logic]") {
  GIVEN("An initialized Handle and a NetManager with a simple order") {
    // Base
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
        {"Test", "A1", std::nullopt, vda5050pp::BlockingType::HARD, {}},
    };
    std::vector<vda5050pp::Action> actions2 = {
        {"Test", "A1", std::nullopt, vda5050pp::BlockingType::NONE, {}},
    };
    vda5050pp::Order order1{
        {},
        "testOrder",
        0,
        std::nullopt,
        {test::mkNode("N1", 0, true, actions1), test::mkNode("N2", 2, true, {})},
        {test::mkEdge("E1", 1, true, "N1", "N2", {})}};
    vda5050pp::Order order2{
        {},
        "testOrder",
        0,
        std::nullopt,
        {test::mkNode("N1", 0, true, actions2), test::mkNode("N2", 2, true, {})},
        {test::mkEdge("E1", 1, true, "N1", "N2", {})}};

    WHEN("There is no active Order a none blocking instant action I1 is received") {
      state.insertInstantActions({{}, {instant_action_none}});
      net_manager.interceptWithAction(instant_action_none);
      handle.spinAll();
      testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::RUNNING);

      WHEN("An Order beginning with a hard blocking action starts") {
        state.appendOrder(order1);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I1 finishes") {
          test::test_action_handler_by_id.at("I1").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, false);
        }
      }
      WHEN("An Order beginning with a none blocking action starts") {
        state.appendOrder(order2);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I1 finishes") {
          test::test_action_handler_by_id.at("I1").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I1"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, true);
        }
      }
    }
    WHEN("There is no active Order a soft blocking instant action I2 is received") {
      state.insertInstantActions({{}, {instant_action_soft}});
      net_manager.interceptWithAction(instant_action_soft);
      handle.spinAll();
      testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::RUNNING);

      WHEN("An Order beginning with a hard blocking action starts") {
        state.appendOrder(order1);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I2 finishes") {
          test::test_action_handler_by_id.at("I2").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, false);
        }
      }
      WHEN("An Order beginning with a none blocking action starts") {
        state.appendOrder(order2);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I2 finishes") {
          test::test_action_handler_by_id.at("I2").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I2"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, true);
        }
      }
    }
    WHEN("There is no active Order a hard blocking instant action I3 is received") {
      state.insertInstantActions({{}, {instant_action_hard}});
      net_manager.interceptWithAction(instant_action_hard);
      handle.spinAll();
      testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);

      WHEN("An Order beginning with a hard blocking action starts") {
        state.appendOrder(order1);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I3 finishes") {
          test::test_action_handler_by_id.at("I3").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, false);
        }
      }
      WHEN("An Order beginning with a none blocking action starts") {
        state.appendOrder(order2);
        net_manager.interpret();
        handle.spinAll();
        testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::RUNNING);
        testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::WAITING);
        testDriving(handle, false);

        WHEN("I3 finishes") {
          test::test_action_handler_by_id.at("I3").get().doFinished();
          handle.spinAll();
          testActionStatus(handle, {"I3"}, vda5050pp::ActionStatus::FINISHED);
          testActionStatus(handle, {"A1"}, vda5050pp::ActionStatus::RUNNING);
          testDriving(handle, true);
        }
      }
    }
  }
}