// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains tests for the BlockingQueue class
//

#include "vda5050++/core/common/blocking_queue.h"

#include <algorithm>
#include <atomic>
#include <catch2/catch.hpp>
#include <chrono>
#include <functional>
#include <list>
#include <optional>
#include <thread>

TEST_CASE("core::common::BlockingQueue synchronized push and pop", "[core::common::BlockingQueue") {
  GIVEN("An empty BlockingQueue") {
    vda5050pp::core::common::BlockingQueue<int> queue;

    int elem1 = 1;
    int elem2 = 2;

    WHEN("Two elements get pushed") {
      queue.push(elem1);
      queue.push(elem2);

      THEN("front() and back() return elem1 and elem2") {
        REQUIRE(queue.front() == elem1);
        REQUIRE(queue.back() == elem2);
      }
    }

    WHEN("a thread wants to pop a value") {
      std::atomic<int> popped = 0;
      std::thread thread1([&] {
        using namespace std::chrono_literals;
        auto opt = queue.try_pop_for(1ms);
        if (opt) popped = *opt;
      });

      THEN("the thread blocks") {
        thread1.join();
        REQUIRE(popped == 0);
      }

      WHEN("a value is pushed") {
        queue.push(elem1);
        thread1.join();
        THEN("the thread pops this value") { REQUIRE(popped == elem1); }
      }
    }
  }
}

template <typename QueueT> bool containsSame(QueueT &lhs, QueueT &rhs) {
  bool same = lhs.size() == rhs.size();

  while (same && !lhs.empty()) {
    same = lhs.front() == rhs.front();
    lhs.pop();
    rhs.pop();
  }

  return same;
}

TEST_CASE("core::common::BlockingQueue copy and move", "[core::common::BlockingQueue") {
  GIVEN("A queue with some values") {
    vda5050pp::core::common::BlockingQueue<int> queue;

    std::list<int> values = {1, 2, 3, 4};

    for (const auto &elem : values) {
      queue.push(elem);
    }

    WHEN("The queue is copied") {
      auto queue_copy = queue;

      THEN("it contains the same elements") { REQUIRE(containsSame(queue_copy, queue)); }
    }

    WHEN("The queue is moved") {
      auto queue_copy = queue;
      auto queue_moved = std::move(queue);

      THEN("it contains the same elements") { REQUIRE(containsSame(queue_moved, queue_copy)); }
    }
  }
}