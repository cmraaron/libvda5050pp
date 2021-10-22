// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/order_id_validator.h"

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/dump.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/interface_agv/handle.h"

TEST_CASE("core::validation - OrderIdValidator", "[core][validation]") {
  GIVEN("A handle with an order") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    vda5050pp::Order order = {{}, "order1", 1, std::nullopt, {}, {}};
    handle_accessor.getState().setOrder(order);

    WHEN("A validator validates a new order with same id and seq") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      auto ret = validator(order);
      test::dumpErrors(handle_accessor, ret);

      THEN("No error was returned (ignore msg)") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates a new order with a new id and 0 seq") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      vda5050pp::Order new_order = {{}, "order2", 0, std::nullopt, {}, {}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);

      THEN("No error was returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates a new order with a new id and non 0 seq") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      vda5050pp::Order new_order = {{}, "order2", 1, std::nullopt, {}, {}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);

      THEN("An error was returned") { REQUIRE_FALSE(ret.empty()); }
    }

    WHEN("A validator validates a new order with the same id and seq+1") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      vda5050pp::Order new_order = order;
      new_order.orderUpdateId += 1;
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);

      THEN("No error was returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates a new order with the same id and seq+2") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      vda5050pp::Order new_order = order;
      new_order.orderUpdateId += 2;
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);

      THEN("No error was returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates a new order with the same id and seq-1") {
      vda5050pp::core::validation::OrderIdValidator validator(handle);
      vda5050pp::Order new_order = order;
      new_order.orderUpdateId -= 1;
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);

      THEN("An error was returned") { REQUIRE_FALSE(ret.empty()); }
    }
  }
}