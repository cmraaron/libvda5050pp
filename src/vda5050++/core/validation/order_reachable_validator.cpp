// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the implementation of the OrderReachableValidator
//

#include "vda5050++/core/validation/order_reachable_validator.h"

#include "vda5050++/core/common/math/geometry.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::validation;

static const std::string k_description = "First Node of new Order is trivially reachable";

OrderReachableValidator::OrderReachableValidator(vda5050pp::interface_agv::Handle &handle) noexcept(
    true)
    : Validator(k_description), handle_(handle) {}

std::list<vda5050pp::Error> OrderReachableValidator::operator()(
    const vda5050pp::Order &order) const {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  if (order.nodes.empty()) {
    // Not the responsibility of this validator
    return {};
  }

  auto first_node = order.nodes.front();

  if (ha.getState().isIdle() && first_node.sequenceId == 0) {
    // continue validation only if this is a new order
  } else {
    // Not the responsibility of this validator
    return {};
  }

  if (auto lastNodeId = ha.getState().getLastNodeId(); lastNodeId != "") {
    // Use last node based check
    if (lastNodeId == first_node.nodeId) {
      // OK
      return {};
    }
  } else if (auto position = ha.getState().getAGVPosition(); position.has_value() &&
                                                             position->positionInitialized &&
                                                             first_node.nodePosition.has_value()) {
    namespace math = vda5050pp::core::common::math;

    // Use position based check
    math::Vector2<double> agv{position->x, position->y};
    math::Vector2<double> node{first_node.nodePosition->x, first_node.nodePosition->y};

    math::Circle<double> agv_possible_positions{agv, position->deviationRange.value_or(0)};
    math::Circle<double> node_deviation{node,
                                        first_node.nodePosition->allowedDeviationXY.value_or(0)};

    if (math::circleEnclosureOf(node_deviation, agv_possible_positions)) {
      // Position is ok
      if (!first_node.nodePosition->theta.has_value()) {
        // Do not check angle -> ok
        return {};
      }

      auto agv_angle = position->theta;
      auto node_angle = *first_node.nodePosition->theta;
      auto angle_diff = math::angleDifference(node_angle, agv_angle);

      if (angle_diff <= first_node.nodePosition->allowedDeviationTheta.value_or(0)) {
        // Angle ok -> ok
        return {};
      }
    }
  }

  // Checks failed
  vda5050pp::Error error;
  error.errorDescription = "First Node of the Order is not trivially reachable";
  error.errorLevel = vda5050pp::ErrorLevel::WARNING;
  error.errorType = "OrderError";
  error.errorReferences = {{"node.nodeId", first_node.nodeId}, {"node.sequenceId", "0"}};
  return {error};
}