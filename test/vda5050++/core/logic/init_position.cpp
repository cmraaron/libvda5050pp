#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_continuous_navigation_handler.h"
#include "test/test_odometry_handler.h"
#include "test/test_pause_resume_handler.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/version.h"
#include "vda5050++/interface_agv/handle.h"

static void testActionStatus(vda5050pp::interface_agv::Handle &handle, std::string id,
                             vda5050pp::ActionStatus status) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(handle).getState().getStateUnsafe();

  THEN(vda5050pp::core::common::logstring("Action ", id, " is ", status)) {
    auto actual_status = state.action_state_by_id[id].actionStatus;
    REQUIRE(actual_status == status);
  }
}

TEST_CASE("core::logic - initPosition behaviour", "[core][logic][action][initPosition]") {
  GIVEN("An initialized Handle with an odometry handler and a initPosition instant action") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);
    vda5050pp::interface_agv::Handlers<test::TestContinuousNavigationHandler,
                                       test::TestActionHandler, test::TestPauseResumeHandler>
        handlers;

    auto connector = std::make_shared<test::TestConnector>();
    vda5050pp::interface_agv::Handle handle({}, connector, handlers, logger);
    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    auto odom_handler = std::make_shared<test::OdometryHandler>();
    handle.setOdometryHandler(odom_handler);
    handle.spinParallel(2);

    vda5050pp::Action init_position;
    init_position.actionId = "a1";
    init_position.actionType = "initPosition";
    init_position.actionParameters = std::make_optional<std::vector<vda5050pp::ActionParameter>>();
    init_position.actionParameters->push_back({"x", "10"});
    init_position.actionParameters->push_back({"y", "20"});
    init_position.actionParameters->push_back({"theta", "-1"});
    init_position.actionParameters->push_back({"mapId", "testMap"});
    init_position.actionParameters->push_back({"lastNodeId", "n0"});
    init_position.blockingType = vda5050pp::BlockingType::HARD;
    vda5050pp::InstantActions init_position_instant_action;
    init_position_instant_action.header.version = vda5050pp::core::version::current;
    init_position_instant_action.header.timestamp = std::chrono::system_clock::now();
    init_position_instant_action.instantActions = {init_position};

    WHEN("The initPosition instant action is received") {
      using namespace std::chrono_literals;
      odom_handler->blockFutureInitializePositionCalls();
      connector->receiveInstantActions(init_position_instant_action);
      std::this_thread::sleep_for(200ms);

      WHEN("The initPosition call has not returned, yet") {
        testActionStatus(handle, "a1", vda5050pp::ActionStatus::RUNNING);
        WHEN("The initPosition call has returned") {
          odom_handler->unblockInitializePositionCalls();
          std::this_thread::sleep_for(200ms);
          testActionStatus(handle, "a1", vda5050pp::ActionStatus::FINISHED);
        }
      }

      THEN("OdometryHandler::initializePosition was called") {
        REQUIRE(odom_handler->hasLastInitPosition());
        REQUIRE(odom_handler->lastInitPosition().x == 10);
        REQUIRE(odom_handler->lastInitPosition().y == 20);
        REQUIRE(odom_handler->lastInitPosition().theta == -1);
        REQUIRE(odom_handler->lastInitPosition().mapId == "testMap");
      }
      odom_handler->unblockInitializePositionCalls();
    }
  }
}