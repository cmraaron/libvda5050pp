// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/action_manager.h"

#include <catch2/catch.hpp>
#include <sstream>

#include "test/console_logger.h"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/types.h"
#include "vda5050++/interface_agv/handle.h"

template <bool will_succeed, vda5050pp::ActionStatus desired_status, typename MemFn,
          typename InnerFn>
inline void testStateGeneric(vda5050pp::core::logic::ActionManager &mgr,
                             const vda5050pp::Action &action,
                             vda5050pp::interface_agv::Handle &handle, MemFn mgr_function,
                             const std::string &mgr_function_name, InnerFn inner_function) {
  auto &action_state = vda5050pp::core::interface_agv::HandleAccessor(handle)
                           .getState()
                           .getStateUnsafe()
                           .action_state_by_id[action.actionId];
  auto prev_action_status = action_state.actionStatus;

  WHEN("Signaling " + mgr_function_name) {
    if constexpr (!will_succeed) {
      auto ret = mgr_function(mgr);
      THEN("it failed") { REQUIRE_FALSE(ret); }
      THEN("The ActionState is unchanged") {
        REQUIRE(action_state.actionStatus == prev_action_status);
      }
      (void)inner_function;
    } else {
      auto ret = mgr_function(mgr);
      THEN("it succeeded") { REQUIRE(ret); }
      std::stringstream ss;
      ss << "The action state is " << desired_status;
      THEN(ss.str()) { REQUIRE(action_state.actionStatus == desired_status); }
      inner_function();
    }
  }
}

template <bool will_succeed = true, typename InnerFn>
inline void testPaused(vda5050pp::core::logic::ActionManager &mgr, const vda5050pp::Action &action,
                       vda5050pp::interface_agv::Handle &handle, InnerFn inner_function) {
  testStateGeneric<will_succeed, vda5050pp::ActionStatus::PAUSED>(
      mgr, action, handle, std::mem_fn(&vda5050pp::core::logic::ActionManager::paused), "paused()",
      inner_function);
}

template <bool will_succeed = true, typename InnerFn>
inline void testResumed(vda5050pp::core::logic::ActionManager &mgr, const vda5050pp::Action &action,
                        vda5050pp::interface_agv::Handle &handle, InnerFn inner_function) {
  testStateGeneric<will_succeed, vda5050pp::ActionStatus::RUNNING>(
      mgr, action, handle, std::mem_fn(&vda5050pp::core::logic::ActionManager::resumed),
      "resumed()", inner_function);
}

template <bool will_succeed = true, typename InnerFn>
inline void testStarted(vda5050pp::core::logic::ActionManager &mgr, const vda5050pp::Action &action,
                        vda5050pp::interface_agv::Handle &handle, InnerFn inner_function) {
  testStateGeneric<will_succeed, vda5050pp::ActionStatus::RUNNING>(
      mgr, action, handle, std::mem_fn(&vda5050pp::core::logic::ActionManager::started),
      "started()", inner_function);
}

template <bool will_succeed = true, typename InnerFn>
inline void testFinished(vda5050pp::core::logic::ActionManager &mgr,
                         const vda5050pp::Action &action, vda5050pp::interface_agv::Handle &handle,
                         InnerFn inner_function) {
  testStateGeneric<will_succeed, vda5050pp::ActionStatus::FINISHED>(
      mgr, action, handle, std::mem_fn(&vda5050pp::core::logic::ActionManager::finished),
      "finished()", inner_function);
}

template <bool will_succeed = true, typename InnerFn>
inline void testFailed(vda5050pp::core::logic::ActionManager &mgr, const vda5050pp::Action &action,
                       vda5050pp::interface_agv::Handle &handle, InnerFn inner_function) {
  testStateGeneric<will_succeed, vda5050pp::ActionStatus::FAILED>(
      mgr, action, handle, std::mem_fn(&vda5050pp::core::logic::ActionManager::failed), "failed()",
      inner_function);
}

TEST_CASE("core::logic::ActionManager combined test", "[core][logic]") {
  GIVEN("An initialized ActionManager, global PTN, Handle (state) and Action") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;

    vda5050pp::interface_agv::Handle handle({}, std::make_shared<test::TestConnector>(), Handlers{},
                                            logger);

    // Create test action and test action state
    vda5050pp::Action action;
    action.actionId = "testId";
    vda5050pp::ActionState action_state;
    action_state.actionId = "testId";
    action_state.actionStatus = vda5050pp::ActionStatus::WAITING;
    auto &state =
        vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();
    state.action_state_by_id[action.actionId] = action_state;

    vda5050pp::core::logic::ActionManager action_manager(handle, action, 0);

    vda5050pp::core::logic::Net global_net;

    // Connect action_manager to the net
    action_manager.attachToNet(global_net);

    // Transition to fill the ready place of the partial action manager net
    global_net.addTransition({{vda5050pp::core::logic::LogicTaskNetTypes::k_ready, 100},
                              {},
                              {{action_manager.readyPlace(), 1}}});
    auto fill_ready_place =
        global_net.findTransition({vda5050pp::core::logic::LogicTaskNetTypes::k_ready, 100});

    WHEN("The partial net is not ready") {
      THEN("Ticking the net will keep the actionstate WAITING") {
        global_net.tick();
        global_net.tick();
        auto &state =
            vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();
        REQUIRE(state.action_state_by_id[action.actionId].actionStatus ==
                vda5050pp::ActionStatus::WAITING);
      }
    }

    WHEN("The partial net is ready and the net ticks") {
      fill_ready_place->fire();
      global_net.tick();

      THEN("The action is in the initializing state") {
        REQUIRE(state.action_state_by_id[action.actionId].actionStatus ==
                vda5050pp::ActionStatus::INITIALIZING);
      }

      WHEN("The callbacks are evaluated") {
        handle.spinOnce();
        test::TestActionHandler &ah = test::test_action_handler_by_id.at(action.actionId);
        THEN("The startAction() callback was called") { REQUIRE(ah.timesStartCalled() == 1); }

        auto &m = action_manager;
        auto &a = action;
        auto &h = handle;

        auto testFailAndPostCond = [&] {
          testFailed<true>(m, a, h, [&] {
            /* AND THEN TEST */
            testPaused<false>(m, a, h, [] {});
            testResumed<false>(m, a, h, [] {});
            testStarted<false>(m, a, h, [] {});
            testFinished<false>(m, a, h, [] {});
            testFailed<false>(m, a, h, [] {});
          });
        };

        testPaused<false>(m, a, h, [] {});
        testResumed<false>(m, a, h, [] {});
        testStarted<true>(m, a, h, [&] {
          /* AND THEN TEST */
          testPaused<true>(m, a, h, [&] {
            /* AND THEN TEST */
            testPaused<false>(m, a, h, [] {});
            testResumed<true>(m, a, h, [] {});
            testStarted<false>(m, a, h, [] {});
            testFinished<false>(m, a, h, [] {});
            testFailAndPostCond();
          });
          testResumed<false>(m, a, h, [] {});
          testStarted<false>(m, a, h, [] {});
          testFinished<true>(m, a, h, [&] {
            /* AND THEN TEST */
            testPaused<false>(m, a, h, [] {});
            testResumed<false>(m, a, h, [] {});
            testStarted<false>(m, a, h, [] {});
            testFinished<false>(m, a, h, [] {});
            testFailed<false>(m, a, h, [] {});
          });
          testFailAndPostCond();
        });
        testFinished<false>(m, a, h, [] {});
        testFailAndPostCond();
      }
    }
  }
}