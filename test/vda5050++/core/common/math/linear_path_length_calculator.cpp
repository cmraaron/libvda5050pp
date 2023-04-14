// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/common/math/linear_path_length_calculator.h"

#include <catch2/catch.hpp>

TEST_CASE("core::common::math::LinearPathCalculator - path calculation", "[core][common][math]") {
  GIVEN("Some linear path calculator and some vertices") {
    using namespace vda5050pp::core::common;

    math::LinearPathLengthCalculator<double> calculator_0;
    math::LinearPathLengthCalculator<double> calculator_bias_10(10);
    math::LinearPathLengthCalculator<double> calculator_init_1_1({1, 1});

    math::Vector2<double> v1{2, 2};
    math::Vector2<double> v2{3, 3};
    math::Vector2<double> v3{-3, 5};

    WHEN("A calculator slurps 3 vertices") {
      auto required_0 = math::euclidDistance(v1, v2) + math::euclidDistance(v2, v3);
      auto required_bias_10 = math::euclidDistance(v1, v2) + math::euclidDistance(v2, v3) + 10;
      auto required_init_1_1 = math::euclidDistance({1, 1}, v1) + math::euclidDistance(v1, v2) +
                               math::euclidDistance(v2, v3);

      calculator_0.addVertices({v1, v2, v3});
      calculator_bias_10.addVertices({v1, v2, v3});
      calculator_init_1_1.addVertices({v1, v2, v3});

      THEN("The length is correct") {
        REQUIRE(required_0 == calculator_0.getLength());
        REQUIRE(required_bias_10 == calculator_bias_10.getLength());
        REQUIRE(required_init_1_1 == calculator_init_1_1.getLength());
      }

      WHEN("A calculator is reset") {
        calculator_0.reset();
        calculator_bias_10.reset();
        calculator_init_1_1.reset();
        THEN("The length is 0") {
          REQUIRE(0 == calculator_0.getLength());
          REQUIRE(0 == calculator_bias_10.getLength());
          REQUIRE(0 == calculator_init_1_1.getLength());
        }

        WHEN("One vertex is added") {
          calculator_0.addVertex(v1);
          THEN("The length is 0") { REQUIRE(0 == calculator_0.getLength()); }
          WHEN("Another vertex is added") {
            calculator_0.addVertex(v2);
            THEN("The distance is correct") {
              auto required = math::euclidDistance(v1, v2);
              REQUIRE(required == calculator_0.getLength());
            }
          }
        }
      }
    }
  }
}