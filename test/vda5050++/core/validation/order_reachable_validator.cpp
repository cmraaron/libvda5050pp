// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains tests for the OrderReachableValidator
//

#include "vda5050++/core/validation/order_reachable_validator.h"

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/dump.hpp"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

TEST_CASE("core::validation - OrderReachableValidator", "[core][validation]") {
  GIVEN("A handle without a position, but last nodeId = n1") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    auto n1 = test::mkNode("Node1", 0, true, {});
    auto n2 = test::mkNode("Node2", 0, true, {});

    handle_accessor.getState().getStateUnsafe().state.lastNodeId = n1.nodeId;

    vda5050pp::Order order_ok{{}, "order_o", 0, {}, {n1}, {}};
    vda5050pp::Order order_error{{}, "order_e", 0, {}, {n2}, {}};

    WHEN("An order starting with n1 is validated") {
      vda5050pp::core::validation::OrderReachableValidator validator(handle);
      auto ret = validator(order_ok);
      test::dumpErrors(handle_accessor, ret);

      THEN("It returns no error") { REQUIRE(ret.empty()); }
    }

    WHEN("An order starting with n2 is validated") {
      vda5050pp::core::validation::OrderReachableValidator validator(handle);
      auto ret = validator(order_error);
      test::dumpErrors(handle_accessor, ret);
      THEN("It returns an error") { REQUIRE_FALSE(ret.empty()); }
    }
  }

  GIVEN("A handle with a position") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    auto n1 = test::mkNode("Node1", 0, true, {});
    auto n2 = test::mkNode("Node2", 0, true, {});

    n1.nodePosition = vda5050pp::NodePosition{};
    n1.nodePosition->x = 10;
    n1.nodePosition->y = 10;
    n1.nodePosition->theta = 0.5;
    n1.nodePosition->allowedDeviationTheta = 0.2;
    n1.nodePosition->allowedDeviationXY = 1;

    n2.nodePosition = vda5050pp::NodePosition{};
    n2.nodePosition->x = 12;
    n2.nodePosition->y = 12;
    n2.nodePosition->theta = 0.5;
    n2.nodePosition->allowedDeviationTheta = 0.2;
    n2.nodePosition->allowedDeviationXY = 1;

    handle_accessor.getState().getStateUnsafe().state.agvPosition = vda5050pp::AGVPosition{};
    handle_accessor.getState().getStateUnsafe().state.agvPosition->x = 10.2;
    handle_accessor.getState().getStateUnsafe().state.agvPosition->y = 10.3;
    handle_accessor.getState().getStateUnsafe().state.agvPosition->theta = 0.45;
    handle_accessor.getState().getStateUnsafe().state.agvPosition->deviationRange = 0.05;
    handle_accessor.getState().getStateUnsafe().state.agvPosition->positionInitialized = true;

    vda5050pp::Order order_ok{{}, "order_o", 0, {}, {n1}, {}};
    vda5050pp::Order order_error{{}, "order_e", 0, {}, {n2}, {}};

    WHEN("An order where the AGV is within the first nodes deviateion is validated") {
      vda5050pp::core::validation::OrderReachableValidator validator(handle);
      auto ret = validator(order_ok);
      test::dumpErrors(handle_accessor, ret);
      THEN("It returns no error") { REQUIRE(ret.empty()); }
    }
    WHEN("An order where the AGV is NOT within the first nodes deviateion is validated") {
      vda5050pp::core::validation::OrderReachableValidator validator(handle);
      auto ret = validator(order_error);
      test::dumpErrors(handle_accessor, ret);
      THEN("It returns an error") { REQUIRE_FALSE(ret.empty()); }
    }
  }
}