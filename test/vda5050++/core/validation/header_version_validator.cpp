// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains tests for the HeaderVersionValidator
//

#include "vda5050++/core/validation/header_version_validator.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/version.h"
#include "vda5050++/interface_agv/handle.h"

using namespace vda5050pp::core;

TEST_CASE("HeaderVersionValidator operates correctly", "[core::validator]") {
  GIVEN("An initialized State and a HeaderVersionValidator") {
    validation::HeaderVersionValidator validator;

    vda5050pp::Header valid_header = {0, std::chrono::system_clock::now(),
                                      std::string(vda5050pp::core::version::current), "test",
                                      "test"};

    vda5050pp::Header invalid_header = {0, std::chrono::system_clock::now(), "31415", "test",
                                        "test"};

    WHEN("A header with compatible version number is verified") {
      auto res = validator(valid_header);

      THEN("No error is returned") { REQUIRE(res.empty()); }
    }

    WHEN("A header with incompatible version number is verified") {
      auto res = validator(invalid_header);

      THEN("An error referencing the version number is returned") {
        REQUIRE_FALSE(res.empty());
        REQUIRE(res.front().errorReferences->size() > 0);
        for (const auto &ref : res.front().errorReferences.value()) {
          REQUIRE(ref.referenceKey == "header.version");
          REQUIRE(ref.referenceValue == invalid_header.version);
        }
      }
    }
  }
}