// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/action_declaration.h"

#include <catch2/catch.h>

#include "vda5050++/model/Action.h"
#include "vda5050++/model/BlockingType.h"

TEST_CASE("core::ActionDeclaration::satisfied_by is working", "[core::ActionDeclaration][util]") {
  using namespace vda5050pp::core;
  GIVEN("Some declarations and actions") {
    ActionDeclaration declaration = {
        "type", {"x", "y", "z"}, {vda5050pp::BlockingType::HARD, vda5050pp::BlockingType::SOFT}};

    vda5050pp::Action only_type_match = {"type", "", "", vda5050pp::BlockingType::NONE, {}};

    vda5050pp::Action no_blocking_match = {
        "type", "", "", vda5050pp::BlockingType::NONE, {{"x", "10"}, {"y", "20"}, {"z", "0"}}};

    vda5050pp::Action no_parameter_match = {
        "type", "", "", vda5050pp::BlockingType::SOFT, {{"x", "10"}, {"yy", "20"}, {"z", "0"}}};

    vda5050pp::Action all_match = {
        "type", "", "", vda5050pp::BlockingType::SOFT, {{"x", "10"}, {"y", "20"}, {"z", "0"}}};

    vda5050pp::Action all_match_2 = {
        "type", "", "", vda5050pp::BlockingType::HARD, {{"x", "10"}, {"y", "20"}, {"z", "0"}}};

    WHEN("Only the type matches") {
      THEN("satisfied_by returns false") {
        REQUIRE_FALSE(declaration.satisfied_by(only_type_match));
      }
    }
    WHEN("The blocking type does not match") {
      THEN("satisfied_by returns false") {
        REQUIRE_FALSE(declaration.satisfied_by(no_blocking_match));
      }
    }
    WHEN("The parameters do not match") {
      THEN("satisfied_by returns false") {
        REQUIRE_FALSE(declaration.satisfied_by(no_parameter_match));
      }
    }
    WHEN("Everything matches") {
      THEN("satisfied_by returns true") {
        REQUIRE(declaration.satisfied_by(all_match));
        REQUIRE(declaration.satisfied_by(all_match_2));
      }
    }
  }
}