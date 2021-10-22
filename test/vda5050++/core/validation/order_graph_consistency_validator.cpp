// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/order_graph_consistency_validator.h"

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/dump.hpp"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

TEST_CASE("core::validation - OrderGraphConsistency", "[core][validation]") {
  GIVEN("A handle without any order") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    WHEN("A valid order is verified") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, false, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, false, "n2", "n3", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("No error was returned") { REQUIRE(ret.empty()); }
    }

    WHEN("An order with interlaced horizon and base") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, false, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, false, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      vda5050pp::Order new_order2 = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, false, "n2", "n3", {})}};

      auto ret1 = validator(new_order);
      auto ret2 = validator(new_order2);
      test::dumpErrors(handle_accessor, ret1);
      test::dumpErrors(handle_accessor, ret2);
      THEN("An error was returned") {
        REQUIRE_FALSE(ret1.empty());
        REQUIRE_FALSE(ret2.empty());
      }
    }

    WHEN("An order with missing sequence id is validated") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      vda5050pp::Order new_order2 = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      auto ret1 = validator(new_order);
      auto ret2 = validator(new_order2);
      test::dumpErrors(handle_accessor, ret1);
      test::dumpErrors(handle_accessor, ret2);
      THEN("An error was returned") {
        REQUIRE_FALSE(ret1.empty());
        REQUIRE_FALSE(ret2.empty());
      }
    }

    WHEN("An order with duplicate sequence id is validated") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {}), test::mkNode("nE", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      vda5050pp::Order new_order2 = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}), test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("eE", 1, true, "n2", "n3", {})}};
      auto ret1 = validator(new_order);
      auto ret2 = validator(new_order2);
      test::dumpErrors(handle_accessor, ret1);
      test::dumpErrors(handle_accessor, ret2);
      THEN("An error was returned") {
        REQUIRE_FALSE(ret1.empty());
        REQUIRE_FALSE(ret2.empty());
      }
    }

    WHEN("An order with invalid sequence id parities is validated") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 1, true, {}), test::mkNode("n2", 3, true, {}),
           test::mkNode("n3", 5, true, {})},
          {test::mkEdge("e1", 2, true, "n1", "n2", {}),
           test::mkEdge("e2", 4, true, "n2", "n3", {})}};
      vda5050pp::Order new_order2 = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 1, true, {}), test::mkNode("n2", 3, true, {}),
           test::mkNode("n3", 5, true, {})},
          {test::mkEdge("e1", 2, true, "n1", "n2", {}),
           test::mkEdge("e2", 4, true, "n2", "n3", {})}};
      auto ret1 = validator(new_order);
      auto ret2 = validator(new_order2);
      test::dumpErrors(handle_accessor, ret1);
      test::dumpErrors(handle_accessor, ret2);
      THEN("An error was returned") {
        REQUIRE_FALSE(ret1.empty());
        REQUIRE_FALSE(ret2.empty());
      }
    }

    WHEN("A valid Order with an unordered list is validated") {
      vda5050pp::core::validation::OrderGraphConsistencyValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n3", 4, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n1", 0, true, {})},
          {test::mkEdge("e2", 3, true, "n1", "n2", {}),
           test::mkEdge("e1", 1, true, "n2", "n3", {})}};
      vda5050pp::Order new_order2 = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n3", 4, true, {}),
           test::mkNode("n2", 2, true, {})},
          {test::mkEdge("e2", 3, true, "n1", "n2", {}),
           test::mkEdge("e1", 1, true, "n2", "n3", {})}};
      auto ret1 = validator(new_order);
      auto ret2 = validator(new_order2);
      test::dumpErrors(handle_accessor, ret1);
      test::dumpErrors(handle_accessor, ret2);
      THEN("No error was returned") {
        REQUIRE(ret1.empty());
        REQUIRE(ret2.empty());
      }
    }
  }
}
