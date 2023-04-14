// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/common/math/geometry.h"

#include <catch2/catch.hpp>

TEST_CASE("common::math::norm", "[common][math][geometry]") {
  GIVEN("Some vectors") {
    using namespace vda5050pp::core::common;
    math::Vector2<double> unit_1 = {1, 0};
    math::Vector2<double> unit_2 = {0, 1};
    math::Vector2<double> vec_5 = {3, 4};
    math::Vector2<double> vec_sqrt_2 = {1, 1};

    WHEN("Norm of {1,0} is calculated") {
      THEN("1 is returned") { REQUIRE(math::norm(unit_1) == 1); }
    }
    WHEN("Norm of {0,1} is calculated") {
      THEN("1 is returned") { REQUIRE(math::norm(unit_2) == 1); }
    }
    WHEN("Norm of {3,4} is calculated") {
      THEN("5 is returned") { REQUIRE(math::norm(vec_5) == 5); }
    }
    WHEN("Norm of {1,1} is calculated") {
      THEN("sqrt(2) is returned") { REQUIRE(math::norm(vec_sqrt_2) == std::sqrt(2)); }
    }
  }
}

TEST_CASE("common::math::euclidDistance", "[common][math][geometry]") {
  GIVEN("Some vecors") {
    using namespace vda5050pp::core::common;
    math::Vector2<double> a = {1, 0};
    math::Vector2<double> b = {2, 0};
    math::Vector2<double> c = {4, 5};
    math::Vector2<double> d = {1, 1};

    WHEN("Distance between {1,0} {2,0} is calculated") {
      THEN("1 is returned") {
        REQUIRE(math::euclidDistance(a, b) == 1);
        REQUIRE(math::euclidDistance(b, a) == 1);
      }
    }
    WHEN("Distance between {1,0} {1,0} is calculated") {
      THEN("0 is returned") { REQUIRE(math::euclidDistance(a, a) == 0); }
    }
    WHEN("Distance between {4,5} {1,1} is calculated") {
      THEN("5 is returned") { REQUIRE(math::euclidDistance(c, d) == 5); }
      THEN("5 is returned") { REQUIRE(math::euclidDistance(d, c) == 5); }
    }
    WHEN("Distance between {1,1} {1,0} is calculated") {
      THEN("1 is returned") { REQUIRE(math::euclidDistance(a, d) == 1); }
      THEN("1 is returned") { REQUIRE(math::euclidDistance(d, a) == 1); }
    }
  }
}

TEST_CASE("common::math::circleIntersection", "[common][math][geometry]") {
  GIVEN("some circes") {
    using namespace vda5050pp::core::common;
    math::Circle<double> a = {{0, 0}, 2};
    math::Circle<double> b = {{4, 0}, 2};
    math::Circle<double> c = {{6, 5}, 2.4};
    math::Circle<double> d = {{2, 2}, 2.4};

    WHEN("Two non intersecting circles are checked") {
      THEN("circleIntersect returns false") {
        REQUIRE_FALSE(math::circleIntersection(a, c));
        REQUIRE_FALSE(math::circleIntersection(c, a));
        REQUIRE_FALSE(math::circleIntersection(c, d));
        REQUIRE_FALSE(math::circleIntersection(c, d));
      }
    }
    WHEN("Two intersecting circles are checked") {
      THEN("circleIntersect returns false") {
        REQUIRE(math::circleIntersection(a, a));
        REQUIRE(math::circleIntersection(b, a));
        REQUIRE(math::circleIntersection(a, a));
        REQUIRE(math::circleIntersection(a, d));
        REQUIRE(math::circleIntersection(d, a));
      }
    }
  }
}

TEST_CASE("common::math::circleEnclosureOf", "[common][math][geometry]") {
  GIVEN("some circes") {
    using namespace vda5050pp::core::common;
    math::Circle<double> a = {{0, 0}, 4};
    math::Circle<double> b = {{2, 0}, 2};
    math::Circle<double> c = {{0, 2}, 2};
    math::Circle<double> d = {{0, 2}, 2.1};
    math::Circle<double> e = {{0, 2.1}, 2};
    math::Circle<double> f = {{0, 0}, 4};
    math::Circle<double> g = {{0, 0}, 4.00001};
    math::Circle<double> h = {{4, 4}, 0.00001};

    WHEN("A circle encloses another circle") {
      THEN("circleEnclosureOf returns true") {
        REQUIRE(math::circleEnclosureOf(a, b));
        REQUIRE(math::circleEnclosureOf(a, c));
        REQUIRE(math::circleEnclosureOf(a, f));
      }
      THEN("circleEnclosureOf returns false for reordered parameters") {
        REQUIRE_FALSE(math::circleEnclosureOf(b, a));
        REQUIRE_FALSE(math::circleEnclosureOf(c, a));
      }
    }
    WHEN("A circle does not enclose another circle") {
      THEN("circleEnclosureOf returns false") {
        REQUIRE_FALSE(math::circleEnclosureOf(a, d));
        REQUIRE_FALSE(math::circleEnclosureOf(a, e));
        REQUIRE_FALSE(math::circleEnclosureOf(a, g));
        REQUIRE_FALSE(math::circleEnclosureOf(a, h));
      }
    }
  }
}