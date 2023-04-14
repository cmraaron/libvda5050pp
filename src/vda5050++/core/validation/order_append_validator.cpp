// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/validation/order_append_validator.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::validation;

static const std::string k_description = "Order appends to the current Order";

OrderAppendValidator::OrderAppendValidator(vda5050pp::interface_agv::Handle &handle) noexcept(true)
    : Validator(k_description), handle_(handle) {}

std::list<vda5050pp::Error> OrderAppendValidator::operator()(const vda5050pp::Order &order) const {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();

  // Do not validate non-appending orders
  if (order.orderId == state.getOrderId() && order.orderUpdateId <= state.getOrderUpdateId()) {
    return {};
  }

  // The Order may replace
  bool may_replace = state.isIdle();

  if (!may_replace && order.orderId != state.getOrderId()) {
    return {{"OrderIDError",
             {{{"order.orderId", order.orderId}, {"state.orderId", state.getOrderId()}}},
             "Order ID does not match the current (unfinished) one",
             ErrorLevel::WARNING}};
  }

  // This case will not be checked here
  if (order.nodes.empty()) {
    return {};
  }

  auto cmp_seq = [](auto &n1, auto &n2) -> bool { return n1.sequenceId < n2.sequenceId; };
  auto order_first_node_it = std::min_element(cbegin(order.nodes), cend(order.nodes), cmp_seq);
  auto min_seq = order_first_node_it->sequenceId;

  bool appends = state.getGraphBaseSeqId() == min_seq;

  // Only allow direct appending
  auto ok = appends || (may_replace && min_seq == 0);
  if (!ok) {
    return {{"OrderStitchingError",
             {{{"order.orderId", order.orderId},
               {"order.orderUpdateId", std::to_string(order.orderUpdateId)},
               {"order.node.sequenceId", std::to_string(order_first_node_it->sequenceId)},
               {"state.baseSequenceId", std::to_string(state.getGraphBaseSeqId())}}},
             "Could not stitch order due to invalid sequence ids",
             ErrorLevel::WARNING}};
  }

  return {};
}