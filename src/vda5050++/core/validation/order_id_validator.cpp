// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
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
      return {{"orderUpdateError",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
               "An order with this orderId and an greater orderUpdateId was already received",
               ErrorLevel::WARNING}};
    }
  } else if (order.orderUpdateId > 0) {
    return {{"orderUpdateError",
             {{{"order.orderId", order.orderId},
               {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
             "An order with this orderId was not received",
             ErrorLevel::WARNING}};
  }

  return {};
}