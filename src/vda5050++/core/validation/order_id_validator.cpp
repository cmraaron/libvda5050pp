// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/validation/order_id_validator.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::validation;

static const std::string k_description = "OrderID and OrderUpdateID are consistent";

OrderIdValidator::OrderIdValidator(vda5050pp::interface_agv::Handle &handle) noexcept(true)
    : Validator(k_description), handle_(handle) {}

std::list<vda5050pp::Error> OrderIdValidator::operator()(const vda5050pp::Order &order) const {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();

  if (order.orderId == state.getOrderId()) {
    if (order.orderUpdateId < state.getOrderUpdateId()) {
      return {{"NewerOrder",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
               "An order with this orderId and an greater orderUpdateId was already received",
               ErrorLevel::WARNING}};
    }
  } else if (order.orderUpdateId > 0) {
    return {{"NoSuchOrder",
             {{{"order.orderId", order.orderId},
               {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
             "An order with this orderId was not received",
             ErrorLevel::WARNING}};
  }

  return {};
}