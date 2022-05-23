// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// TODO: Short description
//

#include "vda5050++/core/common/interruptable_timer.h"

#include <catch2/catch.hpp>
#include <thread>

TEST_CASE("vda5050pp::core::common::InterruptableTimer - uninterrupted sleep", "[thread]") {
  GIVEN("A timer") {
    using namespace std::chrono_literals;
    vda5050pp::core::common::InterruptableTimer timer;

    WHEN("A thread sleeps") {
      vda5050pp::core::common::InterruptableTimerStatus status;
      bool returned = false;
      std::thread thread([&timer, &status, &returned] {
        status = timer.sleepFor(300ms);
        returned = true;
      });

      THEN("After 10ms the thread did not return") {
        std::this_thread::sleep_for(10ms);
        REQUIRE_FALSE(returned);
        thread.join();
      }
      THEN("ok was returned after the time") {
        thread.join();
        REQUIRE(returned);
        REQUIRE(status == vda5050pp::core::common::InterruptableTimerStatus::k_ok);
      }
    }
  }
}

TEST_CASE("vda5050pp::core::common::InterruptableTimer - interruptAll", "[thread]") {
  GIVEN("A timer and multiple sleeping threads") {
    using namespace std::chrono_literals;
    vda5050pp::core::common::InterruptableTimer timer;

    bool ret1 = false;
    vda5050pp::core::common::InterruptableTimerStatus s1;
    bool ret2 = false;
    vda5050pp::core::common::InterruptableTimerStatus s2;
    bool ret3 = false;
    vda5050pp::core::common::InterruptableTimerStatus s3;
    bool ret4 = false;
    vda5050pp::core::common::InterruptableTimerStatus s4;

    std::thread thread1([&timer, &ret1, &s1] {
      s1 = timer.sleepFor(300ms);
      ret1 = true;
    });
    std::thread thread2([&timer, &ret2, &s2] {
      s2 = timer.sleepFor(300ms);
      ret2 = true;
    });
    std::thread thread3([&timer, &ret3, &s3] {
      s3 = timer.sleepFor(300ms);
      ret3 = true;
    });
    std::thread thread4([&timer, &ret4, &s4] {
      s4 = timer.sleepFor(300ms);
      ret4 = true;
    });

    WHEN("interruptAll() was called") {
      std::this_thread::sleep_for(100ms);
      timer.interruptAll();
      timer.waitForClearance();
      THEN("each thread returned") {
        REQUIRE(ret1);
        REQUIRE(ret2);
        REQUIRE(ret3);
        REQUIRE(ret4);
        REQUIRE(s1 == vda5050pp::core::common::InterruptableTimerStatus::k_interrupted);
        REQUIRE(s2 == vda5050pp::core::common::InterruptableTimerStatus::k_interrupted);
        REQUIRE(s3 == vda5050pp::core::common::InterruptableTimerStatus::k_interrupted);
        REQUIRE(s4 == vda5050pp::core::common::InterruptableTimerStatus::k_interrupted);
        thread1.join();
        thread2.join();
        thread3.join();
        thread4.join();

        WHEN("a new thread calls sleep") {
          vda5050pp::core::common::InterruptableTimerStatus s;
          bool r = false;
          std::thread t([&timer, &s, &r] {
            s = timer.sleepFor(300ms);
            r = true;
          });
          THEN("It properly sleeps") {
            REQUIRE_FALSE(r);
            t.join();
            REQUIRE(r);
            REQUIRE(s == vda5050pp::core::common::InterruptableTimerStatus::k_ok);
          }
        }
      }
    }
  }
}

TEST_CASE("vda5050pp::core::common::InterruptableTimer - disable", "[thread]") {
  GIVEN("A disabled timer") {
    using namespace std::chrono_literals;
    vda5050pp::core::common::InterruptableTimer timer;

    timer.disable();

    WHEN("A thread sleeps") {
      auto ret = timer.sleepFor(10ms);

      THEN("Disabled is returned") {
        REQUIRE(ret == vda5050pp::core::common::InterruptableTimerStatus::k_disabled);
      }
    }
  }
}