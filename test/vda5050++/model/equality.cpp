// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains tests for the model/* equality operators
//

#include <catch2/catch.hpp>
#include <optional>

#include "vda5050++/model/Action.h"

TEST_CASE("model::Action equality", "[vda5050pp::Action], [model/equality]") {
  GIVEN("Three actions") {
    vda5050pp::Action a1 = {"t1",
                            "id1",
                            std::nullopt,
                            vda5050pp::BlockingType::NONE,
                            {{{"key1", "false"}, {"key2", "true"}}}};
    vda5050pp::Action a2 = {"t1",
                            "id1",
                            std::nullopt,
                            vda5050pp::BlockingType::NONE,
                            {{{"key1", "false"}, {"key3", "true"}}}};
    vda5050pp::Action a3 = {"t2", "id3", std::nullopt, vda5050pp::BlockingType::NONE, {}};

    WHEN("Two equal actions are compared") {
      THEN("== returns true") { REQUIRE(a1 == a1); }
      THEN("!= returns false") { REQUIRE_FALSE(a1 != a1); }
    }
    WHEN("Two unequal actions are compared") {
      THEN("== returns false") {
        REQUIRE_FALSE(a1 == a2);
        REQUIRE_FALSE(a1 == a3);
      }
      THEN("!= returns true") {
        REQUIRE(a1 != a2);
        REQUIRE(a1 != a3);
      }
    }
  }
}