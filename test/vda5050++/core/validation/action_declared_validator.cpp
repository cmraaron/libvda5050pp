// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the tests for the ActionDeclararationValidator
//

#include "vda5050++/core/validation/action_declared_validator.h"

#include <catch2/catch.hpp>

#include "test/console_logger.h"
#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/interface_agv/handle.h"

using SV = vda5050pp::interface_agv::agv_description::SerializedValue;

vda5050pp::interface_agv::agv_description::ActionDeclaration decl_honk = {
    "honk",
    {{"volume", std::make_optional(SV(int64_t(0))), std::make_optional(SV(int64_t(100))),
      std::nullopt}},
    {{"sound", std::nullopt, std::nullopt, {{"a", "b", "c"}}}},
    {vda5050pp::BlockingType::NONE, vda5050pp::BlockingType::SOFT},
    true,
    true,
    true};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_pick = {
    "pick", {{"place", std::nullopt, std::nullopt, {{"front", "rear"}}}},
    {},     {vda5050pp::BlockingType::HARD},
    false,  true,
    false};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_edge = {
    "edge", {}, {}, {vda5050pp::BlockingType::NONE}, false, false, true};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_node = {
    "node", {}, {}, {vda5050pp::BlockingType::NONE}, false, true, false};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_instant = {
    "instant", {}, {}, {vda5050pp::BlockingType::NONE}, true, false, false};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_hard_soft = {
    "hard_soft", {},    {},   {vda5050pp::BlockingType::HARD, vda5050pp::BlockingType::SOFT},
    true,        false, false};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_soft_none = {
    "soft_none", {},    {},   {vda5050pp::BlockingType::NONE, vda5050pp::BlockingType::SOFT},
    true,        false, false};
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_none = {
    "none", {}, {}, {vda5050pp::BlockingType::NONE}, true, false, false};
vda5050pp::interface_agv::agv_description::AGVDescription description;
vda5050pp::interface_agv::agv_description::ActionDeclaration decl_ord_str = {
    "ord_str", {{"string", SV(std::string("AA")), SV(std::string("AX")), std::nullopt}},
    {},        {vda5050pp::BlockingType::NONE},
    true,      false,
    false};

TEST_CASE("core::validation::ActionDeclared - context check", "[core][validation]") {
  GIVEN("An AGVDescription, a validator for edge, instant or node actions") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    description.supported_actions = {decl_node, decl_edge, decl_instant};
    vda5050pp::interface_agv::Handle handle(
        description, std::make_shared<test::TestConnector>(),
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>{},
        logger);

    vda5050pp::core::validation::ActionDeclaredValidator validator_edge(handle, true, false, false);
    vda5050pp::core::validation::ActionDeclaredValidator validator_instant(handle, false, true,
                                                                           false);
    vda5050pp::core::validation::ActionDeclaredValidator validator_node(handle, false, false, true);

    WHEN("An Edge action is validated") {
      vda5050pp::Action action = {"edge", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_e = validator_edge(action);
      auto res_n = validator_node(action);
      auto res_i = validator_instant(action);

      THEN("Only the edge validator succeeds") {
        REQUIRE(res_e.empty());
        REQUIRE_FALSE(res_n.empty());
        REQUIRE_FALSE(res_i.empty());
      }
    }
    WHEN("An Node action is validated") {
      vda5050pp::Action action = {"node", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_e = validator_edge(action);
      auto res_n = validator_node(action);
      auto res_i = validator_instant(action);

      THEN("Only the node validator succeeds") {
        REQUIRE(res_n.empty());
        REQUIRE_FALSE(res_e.empty());
        REQUIRE_FALSE(res_i.empty());
      }
    }
    WHEN("An Instant action is validated") {
      vda5050pp::Action action = {"instant", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_e = validator_edge(action);
      auto res_n = validator_node(action);
      auto res_i = validator_instant(action);

      THEN("Only the instant validator succeeds") {
        REQUIRE(res_i.empty());
        REQUIRE_FALSE(res_e.empty());
        REQUIRE_FALSE(res_n.empty());
      }
    }
  }
}

TEST_CASE("core::validation::ActionDeclared - blocking check", "[core][validation]") {
  GIVEN("An AGVDescription and a validator") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    description.supported_actions = {decl_hard_soft, decl_soft_none, decl_none};
    vda5050pp::interface_agv::Handle handle(
        description, std::make_shared<test::TestConnector>(),
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>{},
        logger);

    vda5050pp::core::validation::ActionDeclaredValidator validator(handle, false, false, false);

    WHEN("Blockingtypes match (hard,soft)") {
      vda5050pp::Action action_h = {
          "hard_soft", "id", std::nullopt, vda5050pp::BlockingType::HARD, {}};
      vda5050pp::Action action_s = {
          "hard_soft", "id", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
      vda5050pp::Action action_n = {
          "hard_soft", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_h = validator(action_h);
      auto res_s = validator(action_s);
      auto res_n = validator(action_n);

      THEN("Only the NONE blocking action fails") {
        REQUIRE(res_h.empty());
        REQUIRE(res_s.empty());
        REQUIRE_FALSE(res_n.empty());
      }
    }
    WHEN("Blockingtypes match (none,soft)") {
      vda5050pp::Action action_h = {
          "soft_none", "id", std::nullopt, vda5050pp::BlockingType::HARD, {}};
      vda5050pp::Action action_s = {
          "soft_none", "id", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
      vda5050pp::Action action_n = {
          "soft_none", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_h = validator(action_h);
      auto res_s = validator(action_s);
      auto res_n = validator(action_n);

      THEN("Only the HARD blocking action fails") {
        REQUIRE_FALSE(res_h.empty());
        REQUIRE(res_s.empty());
        REQUIRE(res_n.empty());
      }
    }
    WHEN("Blockingtypes match (none)") {
      vda5050pp::Action action_h = {"none", "id", std::nullopt, vda5050pp::BlockingType::HARD, {}};
      vda5050pp::Action action_s = {"none", "id", std::nullopt, vda5050pp::BlockingType::SOFT, {}};
      vda5050pp::Action action_n = {"none", "id", std::nullopt, vda5050pp::BlockingType::NONE, {}};

      auto res_h = validator(action_h);
      auto res_s = validator(action_s);
      auto res_n = validator(action_n);

      THEN("Only the NONE blocking action succeeds") {
        REQUIRE_FALSE(res_h.empty());
        REQUIRE_FALSE(res_s.empty());
        REQUIRE(res_n.empty());
      }
    }
  }
}

TEST_CASE("core::validation::ActionDeclared - parameter check", "[core][validation]") {
  GIVEN("An AGVDescription and a validator") {
    auto logger = std::make_shared<test::ConsoleLogger>();
    logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_debug);

    description.supported_actions = {decl_pick, decl_honk, decl_ord_str};
    vda5050pp::interface_agv::Handle handle(
        description, std::make_shared<test::TestConnector>(),
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>{},
        logger);

    vda5050pp::core::validation::ActionDeclaredValidator validator(handle, false, false, false);

    WHEN("An action has an parameter inside of a value set") {
      vda5050pp::Action action1 = {
          "pick", "id", std::nullopt, vda5050pp::BlockingType::HARD, {{{"place", "front"}}}};
      vda5050pp::Action action2 = {
          "pick", "id", std::nullopt, vda5050pp::BlockingType::HARD, {{{"place", "rear"}}}};

      auto res1 = validator(action1);
      auto res2 = validator(action2);
      THEN("It is valid") {
        REQUIRE(res1.empty());
        REQUIRE(res2.empty());
      }
    }

    WHEN("An action has an parameter not inside of a value set") {
      vda5050pp::Action action = {
          "pick", "id", std::nullopt, vda5050pp::BlockingType::HARD, {{{"place", "outside"}}}};

      auto res = validator(action);
      THEN("It is invalid") { REQUIRE_FALSE(res.empty()); }
    }

    WHEN("An action is missing a mandatory parameter") {
      vda5050pp::Action action = {"pick", "id", std::nullopt, vda5050pp::BlockingType::HARD, {}};

      auto res = validator(action);
      THEN("It is invalid") { REQUIRE_FALSE(res.empty()); }
    }

    WHEN("An action is missing an optional parameter") {
      vda5050pp::Action action = {
          "honk", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"volume", "50"}}}};

      auto res = validator(action);
      THEN("It is valid") { REQUIRE(res.empty()); }
    }

    WHEN("An action has a parameter value outside of the ordinal bounds") {
      vda5050pp::Action action1 = {
          "honk", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"volume", "101"}}}};
      vda5050pp::Action action2 = {
          "honk", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"volume", "-1"}}}};
      vda5050pp::Action action3 = {
          "ord_str", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"string", "AZ"}}}};
      vda5050pp::Action action4 = {
          "ord_str", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"string", "01"}}}};

      auto res1 = validator(action1);
      auto res2 = validator(action2);
      auto res3 = validator(action3);
      auto res4 = validator(action4);
      THEN("It is invalid") {
        REQUIRE_FALSE(res1.empty());
        REQUIRE_FALSE(res2.empty());
        REQUIRE_FALSE(res3.empty());
        REQUIRE_FALSE(res4.empty());
      }
    }

    WHEN("An action has a parameter value inside of the ordinal bounds") {
      vda5050pp::Action action1 = {
          "honk", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"volume", "100"}}}};
      vda5050pp::Action action2 = {
          "honk", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"volume", "0"}}}};
      vda5050pp::Action action3 = {
          "ord_str", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"string", "AB"}}}};
      vda5050pp::Action action4 = {
          "ord_str", "id", std::nullopt, vda5050pp::BlockingType::NONE, {{{"string", "AM"}}}};

      auto res1 = validator(action1);
      auto res2 = validator(action2);
      auto res3 = validator(action1);
      auto res4 = validator(action2);
      THEN("It is valid") {
        REQUIRE(res1.empty());
        REQUIRE(res2.empty());
        REQUIRE(res3.empty());
        REQUIRE(res4.empty());
      }
    }
  }
}