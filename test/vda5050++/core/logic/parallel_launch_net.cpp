// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/parallel_launch_net.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/logic/types.h"

TEST_CASE("core::logic::ParallelLaunchNet launch test", "[core][logic]") {
  GIVEN("A net with three places") {
    using namespace vda5050pp::core::logic;

    Net net;

    LogicTaskNetID begin = {LogicTaskNetTypes::k_done, 0};
    std::vector<LogicTaskNetID> targets = {
        {LogicTaskNetTypes::k_ready, 1},
        {LogicTaskNetTypes::k_ready, 2},
        {LogicTaskNetTypes::k_ready, 3},
    };

    net.addPlace(begin, 1);

    for (const auto &id : targets) {
      net.addPlace(id, 0);
    }

    ParallelLaunchNet pln(begin, std::vector(targets));

    WHEN("The pln attaches to the net") {
      THEN("No exception is thrown") {
        REQUIRE_NOTHROW(pln.attachToNet(net));

        WHEN("The net ticks") {
          net.tick();
          THEN("Begin has 0 tokens") { REQUIRE(net.findPlace(begin)->getTokens() == 0); }
          THEN("Targets have 1 tokens") {
            for (const auto &id : targets) {
              REQUIRE(net.findPlace(id)->getTokens() == 1);
            }
          }
        }
      }
    }
  }
}