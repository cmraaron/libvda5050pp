// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/order_append_validator.h"

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/dump.hpp"
#include "test/order_factory.hpp"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

TEST_CASE("core::validation - OrderAppendValidator", "[core][validation]") {
  GIVEN("A handle with an order (without horizon)") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    vda5050pp::Order order = {{},
                              "order1",
                              1,
                              std::nullopt,
                              {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {})},
                              {test::mkEdge("e1", 1, true, "n1", "n2", {})}};
    handle_accessor.getState().setOrder(order);

    WHEN("The AGV is idle again") {
      handle_accessor.getState().setLastNodeReached(2);

      WHEN("A validator validates an appending order (increased seq)") {
        vda5050pp::core::validation::OrderAppendValidator validator(handle);
        vda5050pp::Order new_order = {
            {},
            "order1",
            2,
            std::nullopt,
            {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {}),
             test::mkNode("n4", 6, true, {})},
            {test::mkEdge("e2", 3, true, "n2", "n3", {}),
             test::mkEdge("e3", 5, true, "n3", "n4", {})}};
        auto ret = validator(new_order);
        test::dumpErrors(handle_accessor, ret);
        THEN("No error is returned") { REQUIRE(ret.empty()); }
      }
      WHEN("A validator validates an appending order (new id)") {
        vda5050pp::core::validation::OrderAppendValidator validator(handle);
        vda5050pp::Order new_order = {
            {},
            "order2",
            0,
            std::nullopt,
            {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {}),
             test::mkNode("n4", 6, true, {})},
            {test::mkEdge("e2", 3, true, "n2", "n3", {}),
             test::mkEdge("e3", 5, true, "n3", "n4", {})}};
        auto ret = validator(new_order);
        test::dumpErrors(handle_accessor, ret);
        THEN("No error is returned") { REQUIRE(ret.empty()); }
      }

      WHEN("A validator validates an order with redundant elements") {
        vda5050pp::core::validation::OrderAppendValidator validator(handle);
        vda5050pp::Order new_order = {
            {},
            "order2",
            0,
            std::nullopt,
            {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
             test::mkNode("n3", 4, true, {}), test::mkNode("n4", 6, true, {})},
            {test::mkEdge("e1", 1, true, "n1", "n2", {}),
             test::mkEdge("e2", 3, true, "n2", "n3", {}),
             test::mkEdge("e3", 5, true, "n3", "n4", {})}};
        auto ret = validator(new_order);
        test::dumpErrors(handle_accessor, ret);
        THEN("No error is returned") { REQUIRE(ret.empty()); }
      }

      WHEN("A validator validates an order with missing elements (not repeating the last node)") {
        vda5050pp::core::validation::OrderAppendValidator validator(handle);
        vda5050pp::Order new_order = {
            {},
            "order2",
            0,
            std::nullopt,
            {test::mkNode("n3", 4, true, {}), test::mkNode("n4", 6, true, {})},
            {test::mkEdge("e2", 3, true, "n2", "n3", {}),
             test::mkEdge("e3", 5, true, "n3", "n4", {})}};
        auto ret = validator(new_order);
        test::dumpErrors(handle_accessor, ret);
        THEN("An error is returned") { REQUIRE_FALSE(ret.empty()); }
      }
    }

    WHEN("A validator validates an appending order (increased seq)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          2,
          std::nullopt,
          {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {}),
           test::mkNode("n4", 6, true, {})},
          {test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("e3", 5, true, "n3", "n4", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("No error is returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates an appending order (new id)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order2",
          0,
          std::nullopt,
          {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {}),
           test::mkNode("n4", 6, true, {})},
          {test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("e3", 5, true, "n3", "n4", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("An error is returned") {
        REQUIRE_FALSE(ret.empty());
      }  // new behaviour (reject on different id)
    }

    WHEN("A validator validates an order with redundant elements") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order2",
          0,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {}), test::mkNode("n4", 6, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}), test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("e3", 5, true, "n3", "n4", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("An error is returned") { REQUIRE_FALSE(ret.empty()); }
    }

    WHEN("A validator validates an order with missing elements (not repeating the last node)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order2",
          0,
          std::nullopt,
          {test::mkNode("n3", 4, true, {}), test::mkNode("n4", 6, true, {})},
          {test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("e3", 5, true, "n3", "n4", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("An error is returned") { REQUIRE_FALSE(ret.empty()); }
    }
  }

  GIVEN("A handle without a state") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    WHEN("A validator validates an appending order with starting sequence id > 0") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          0,
          std::nullopt,
          {test::mkNode("n3", 4, true, {}), test::mkNode("n4", 6, true, {})},
          {test::mkEdge("e2", 3, true, "n2", "n3", {}),
           test::mkEdge("e3", 5, true, "n3", "n4", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("An error is returned") { REQUIRE_FALSE(ret.empty()); }
    }
  }

  GIVEN("A handle with an order (with horizon)") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    vda5050pp::interface_agv::agv_description::AGVDescription description;
    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(description, std::make_shared<test::TestConnector>(),
                                            Handlers{}, logger);

    vda5050pp::core::interface_agv::HandleAccessor handle_accessor(handle);

    vda5050pp::Order order = {{},
                              "order1",
                              1,
                              std::nullopt,
                              {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, false, {})},
                              {test::mkEdge("e1", 1, false, "n1", "n2", {})}};
    handle_accessor.getState().setOrder(order);

    WHEN("A validator validates an appending order (increased seq)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          2,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("No error is returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates an appending order (new id)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          2,
          std::nullopt,
          {test::mkNode("n1", 0, true, {}), test::mkNode("n2", 2, true, {}),
           test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("No error is returned") { REQUIRE(ret.empty()); }
    }

    WHEN("A validator validates an order with missing elements (not repeating the last node)") {
      vda5050pp::core::validation::OrderAppendValidator validator(handle);
      vda5050pp::Order new_order = {
          {},
          "order1",
          2,
          std::nullopt,
          {test::mkNode("n2", 2, true, {}), test::mkNode("n3", 4, true, {})},
          {test::mkEdge("e1", 1, true, "n1", "n2", {}),
           test::mkEdge("e2", 3, true, "n2", "n3", {})}};
      auto ret = validator(new_order);
      test::dumpErrors(handle_accessor, ret);
      THEN("An error is returned") { REQUIRE_FALSE(ret.empty()); }
    }
  }
}