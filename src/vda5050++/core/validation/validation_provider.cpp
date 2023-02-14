// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/validation_provider.h"

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/validation/action_declared_validator.h"
#include "vda5050++/core/validation/header_target_validator.h"
#include "vda5050++/core/validation/header_version_validator.h"
#include "vda5050++/core/validation/order_action_validator.h"
#include "vda5050++/core/validation/order_append_validator.h"
#include "vda5050++/core/validation/order_graph_consistency_validator.h"
#include "vda5050++/core/validation/order_id_validator.h"
#include "vda5050++/core/validation/order_reachable_validator.h"

using namespace vda5050pp::core::validation;

ValidationProvider::ValidationProvider(vda5050pp::interface_agv::Handle &handle) : handle_(handle) {
  this->header_validators_.push_back(std::make_unique<HeaderTargetValidator>(handle));
  this->header_validators_.push_back(std::make_unique<HeaderVersionValidator>());
  this->action_edge_validator_ =
      std::make_unique<ActionDeclaredValidator>(handle, true, false, false);
  this->action_instant_validator_ =
      std::make_unique<ActionDeclaredValidator>(handle, false, true, false);
  this->action_node_validator_ =
      std::make_unique<ActionDeclaredValidator>(handle, false, false, true);
  this->order_validators_.push_back(std::make_unique<OrderIdValidator>(handle));
  this->order_validators_.push_back(std::make_unique<OrderAppendValidator>(handle));
  this->order_validators_.push_back(std::make_unique<OrderGraphConsistencyValidator>(handle));
  this->order_validators_.push_back(std::make_unique<OrderActionValidator>(handle));
  this->order_validators_.push_back(std::make_unique<OrderReachableValidator>(handle));
}

template <>
std::list<vda5050pp::Error> ValidationProvider::doValidation(
    const Validator<vda5050pp::Action> &validator, const vda5050pp::Action &obj) const
    noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getLogger().logDebug(vda5050pp::core::common::logstring("Validating Action ", obj.actionId));

  auto res = validator(obj);

  // invalid?
  if (!res.empty()) {
    ha.getLogger().logInfo(vda5050pp::core::common::logstring("Invalid Action (", obj.actionId,
                                                              "): ", validator.what(), " failed."));
  }

  return res;
}

template <>
std::list<vda5050pp::Error> ValidationProvider::doValidation(
    const Validator<vda5050pp::Header> &validator, const vda5050pp::Header &obj) const
    noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getLogger().logDebug(vda5050pp::core::common::logstring("Validating Header ", obj.headerId));

  auto res = validator(obj);

  // invalid?
  if (!res.empty()) {
    ha.getLogger().logInfo(vda5050pp::core::common::logstring("Invalid Header (", obj.headerId,
                                                              "): ", validator.what(), " failed."));
  }

  return res;
}

template <>
std::list<vda5050pp::Error> ValidationProvider::doValidation(
    const Validator<vda5050pp::Order> &validator, const vda5050pp::Order &obj) const
    noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getLogger().logDebug(
      vda5050pp::core::common::logstring("Validating Order ", obj.orderId, "@", obj.orderUpdateId));

  auto res = validator(obj);

  // invalid?
  if (!res.empty()) {
    ha.getLogger().logInfo(vda5050pp::core::common::logstring("Invalid order (", obj.orderId, "@",
                                                              obj.orderUpdateId,
                                                              "): ", validator.what(), " failed."));
  }

  return res;
}

std::list<vda5050pp::Error> ValidationProvider::validateAction(const vda5050pp::Action &action,
                                                               bool ctxt_instant, bool ctxt_edge,
                                                               bool ctxt_node) const
    noexcept(true) {
  if (ctxt_instant) {
    return this->doValidation<vda5050pp::Action>(*this->action_instant_validator_, action);
  } else if (ctxt_edge) {
    return this->doValidation<vda5050pp::Action>(*this->action_edge_validator_, action);
  } else if (ctxt_node) {
    return this->doValidation<vda5050pp::Action>(*this->action_node_validator_, action);
  }

  return {};
}

std::list<vda5050pp::Error> ValidationProvider::validateHeader(
    const vda5050pp::Header &header) const noexcept(true) {
  std::list<vda5050pp::Error> errors;

  for (auto &v_ptr : this->header_validators_) {
    errors.splice(begin(errors), doValidation(*v_ptr, header));
  }

  return errors;
}

std::list<vda5050pp::Error> ValidationProvider::validateInstantActions(
    const vda5050pp::InstantActions &instant_actions) const noexcept(true) {
  std::list<vda5050pp::Error> errors;

  errors.splice(begin(errors), this->validateHeader(instant_actions.header));

  for (const auto &a : instant_actions.actions) {
    errors.splice(begin(errors), this->validateAction(a, true, false, false));
  }

  return errors;
}

std::list<vda5050pp::Error> ValidationProvider::validateOrder(const vda5050pp::Order &order) const
    noexcept(true) {
  std::list<vda5050pp::Error> errors;

  errors.splice(begin(errors), this->validateHeader(order.header));

  for (auto &v_ptr : this->order_validators_) {
    errors.splice(begin(errors), doValidation(*v_ptr, order));
  }

  if (!errors.empty()) {
    vda5050pp::Error error;
    error.errorType = "orderError";
    error.errorDescription = "Order contained Errors (more details -> see errors)";
    error.errorLevel = vda5050pp::ErrorLevel::WARNING;
    error.errorReferences = {
        {"orderId", order.orderId},
        {"orderUpdateId", std::to_string(order.orderUpdateId)},
    };
    errors.push_front(std::move(error));
  }

  return errors;
}

bool ValidationProvider::ignoreOrder(const vda5050pp::Order &order) const noexcept(true) {
  vda5050pp::core::interface_agv::ConstHandleAccessor ha(this->handle_);

  auto id = ha.getState().getOrderId();
  auto update_id = ha.getState().getOrderUpdateId();

  return order.orderId == id && order.orderUpdateId == update_id;
}