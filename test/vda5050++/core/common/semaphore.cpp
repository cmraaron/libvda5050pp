// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains tests for the Semaphore class
//

#include "vda5050++/core/common/semaphore.h"

#include <catch2/catch.hpp>
#include <chrono>
#include <list>
#include <thread>
#include <utility>

TEST_CASE("core::common::Semaphore basic acquire and release", "[core::common::Semaphore") {
  GIVEN("A Semaphore managing 2 resources") {
    vda5050pp::core::common::Semaphore semaphore(2);
    bool use_resource = false;

    WHEN("two resources get acquired and a thread acquires the third") {
      semaphore.acquire();
      semaphore.acquire();

      std::thread thread([&] {
        semaphore.acquire();
        use_resource = true;
        semaphore.release();
      });

      THEN("The thread should not use the resource") {
        REQUIRE_FALSE(use_resource);

        WHEN("the resource is freed") {
          semaphore.release();
          thread.join();
          THEN("The thread should use the resource") { REQUIRE(use_resource); }
        }
      }
    }
  }
}

TEST_CASE("core::common::Semaphore copy and move", "[core::common::Semaphore]") {
  GIVEN("A semaphore with 2 available resources") {
    vda5050pp::core::common::Semaphore sem(2);

    WHEN("The semaphore is copy assigned") {
      auto copy = sem;
      THEN("The avail counter is copied") {
        REQUIRE(copy.try_acquire());
        REQUIRE(copy.try_acquire());
        REQUIRE_FALSE(copy.try_acquire());
      }
    }

    WHEN("The semaphore is move assigned") {
      auto moved = std::move(sem);
      THEN("The avail counter is moved") {
        REQUIRE(moved.try_acquire());
        REQUIRE(moved.try_acquire());
        REQUIRE_FALSE(moved.try_acquire());
      }
    }

    WHEN("The semaphore is copy constructed") {
      vda5050pp::core::common::Semaphore copy(sem);
      THEN("The avail counter is copied") {
        REQUIRE(copy.try_acquire());
        REQUIRE(copy.try_acquire());
        REQUIRE_FALSE(copy.try_acquire());
      }
    }

    WHEN("The semaphore is move constructed") {
      vda5050pp::core::common::Semaphore moved(sem);
      THEN("The avail counter is moved") {
        REQUIRE(moved.try_acquire());
        REQUIRE(moved.try_acquire());
        REQUIRE_FALSE(moved.try_acquire());
      }
    }
  }
}