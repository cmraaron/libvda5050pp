// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains tests for the HeaderTargetValidator
//

#include "vda5050++/core/validation/header_target_validator.h"

#include <catch2/catch.hpp>

#include "test/test_action_handler.h"
#include "test/test_connector.h"
#include "test/test_pause_resume_handler.h"
#include "test/test_step_based_navigation_handler.h"
#include "vda5050++/interface_agv/handle.h"

using namespace vda5050pp::core;

TEST_CASE("HeaderTargetValidator operates correctly", "[core][validation]") {
  GIVEN("An initialized State and a HeaderTargetValidator") {
    vda5050pp::interface_agv::agv_description::AGVDescription desc;
    desc.manufacturer = "Testmanufacturer";
    desc.serial_number = "1234";

    using Handlers =
        vda5050pp::interface_agv::Handlers<test::TestStepBasedNavigationHandler,
                                           test::TestActionHandler, test::TestPauseResumeHandler>;
    vda5050pp::interface_agv::Handle handle(desc, std::make_shared<test::TestConnector>(),
                                            Handlers{}, nullptr);

    validation::HeaderTargetValidator validator(handle);

    vda5050pp::Header valid_header = {0, std::chrono::system_clock::now(), "0.1", desc.manufacturer,
                                      desc.serial_number};

    vda5050pp::Header invalid_sn_header = {0, std::chrono::system_clock::now(), "0.1",
                                           desc.manufacturer, "-1"};

    vda5050pp::Header invalid_manufacturer_header = {0, std::chrono::system_clock::now(), "0.1",
                                                     "meh", desc.serial_number};

    WHEN("The validator validates a Header with correct Manufacturer and SN") {
      auto res = validator(valid_header);
      THEN("No error is returned") { REQUIRE(res.empty()); }
    }

    WHEN("The validator validates a Header with incorrect SN") {
      auto res = validator(invalid_sn_header);
      THEN("An error referencing the sn is returned") {
        REQUIRE_FALSE(res.empty());
        REQUIRE(res.front().errorReferences->size() > 0);
        for (const auto &ref : res.front().errorReferences.value()) {
          REQUIRE(ref.referenceKey == "header.serialNumber");
          REQUIRE(ref.referenceValue == invalid_sn_header.serialNumber);
        }
      }
    }

    WHEN("The validator validates a Header with incorrect Manufacturer") {
      auto res = validator(invalid_manufacturer_header);
      THEN("An error referencing the Manufacturer is returned") {
        REQUIRE_FALSE(res.empty());
        REQUIRE(res.front().errorReferences->size() > 0);
        for (const auto &ref : res.front().errorReferences.value()) {
          REQUIRE(ref.referenceKey == "header.manufacturer");
          REQUIRE(ref.referenceValue == invalid_manufacturer_header.manufacturer);
        }
      }
    }
  }
}