// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/validation/order_action_validator.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::validation;

static const std::string k_description = "Order does not contain any invalid actions";

OrderActionValidator::OrderActionValidator(vda5050pp::interface_agv::Handle &handle) noexcept(true)
    : Validator(k_description), handle_(handle) {}

std::list<vda5050pp::Error> OrderActionValidator::operator()(const vda5050pp::Order &order) const {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &validation_provider = ha.getValidationProvider();

  std::list<vda5050pp::Error> errors;

  // Gather action errors
  for (auto &node : order.nodes) {
    for (auto &action : node.actions) {
      errors.splice(begin(errors), validation_provider.validateAction(action, false, false, true));
    }
  }
  for (auto &edge : order.edges) {
    for (auto &action : edge.actions) {
      errors.splice(begin(errors), validation_provider.validateAction(action, false, true, false));
    }
  }

  return errors;
}