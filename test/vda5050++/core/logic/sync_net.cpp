// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/sync_net.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/logic/types.h"

TEST_CASE("core::logic::SyncNet - construction", "[core][logic]") {
  GIVEN("Some ids") {
    using namespace vda5050pp::core::logic;

    std::vector<LogicTaskNetID> sync_ids = {
        {LogicTaskNetTypes::k_done, 1},
        {LogicTaskNetTypes::k_done, 2},
        {LogicTaskNetTypes::k_done, 3},
        {LogicTaskNetTypes::k_done, 4},
    };

    LogicTaskNetID final_place = {LogicTaskNetTypes::k_ready, 5};
    LogicTaskNetID transition_id = {LogicTaskNetTypes::k_combinator_sync_transition, 5};

    WHEN("An explicit SN is created") {
      SyncNet sn(std::vector<LogicTaskNetID>(sync_ids), final_place);

      THEN("getters return the correct ids") {
        REQUIRE(sn.getPlaceID() == final_place);
        REQUIRE(sn.getTransitionID() == transition_id);
        REQUIRE(sn.getSyncIDs() == sync_ids);
      }
    }
  }
}

TEST_CASE("core::logic::SyncNet - behaviour", "[core][logic]") {
  GIVEN("A net and a SN") {
    using namespace vda5050pp::core::logic;

    std::vector<LogicTaskNetID> sync_ids = {
        {LogicTaskNetTypes::k_done, 1},
        {LogicTaskNetTypes::k_done, 2},
        {LogicTaskNetTypes::k_done, 3},
        {LogicTaskNetTypes::k_done, 4},
    };

    LogicTaskNetID final_place{LogicTaskNetTypes::k_ready, 5};

    Net net;

    for (const auto &id : sync_ids) {
      net.addPlace(id, 1);
    }
    net.addPlace(final_place, 0);

    SyncNet sn(std::vector<LogicTaskNetID>(sync_ids), final_place);

    WHEN("The SN is attached to the net") {
      THEN("No exception is thrown") {
        REQUIRE_NOTHROW(sn.attachToNet(net));

        WHEN("The net ticks") {
          net.tick();

          THEN("The sync places have no tokens") {
            for (const auto &id : sync_ids) {
              REQUIRE(net.findPlace(id)->getTokens() == 0);
            }
          }

          THEN("The final place has one token") {
            REQUIRE(net.findPlace(final_place)->getTokens() == 1);
          }
        }
      }
    }
  }
}