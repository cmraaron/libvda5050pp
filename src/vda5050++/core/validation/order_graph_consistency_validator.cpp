// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/order_graph_consistency_validator.h"

#include <set>

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::validation;

static const std::string k_description = "Order Graph is consistent";

OrderGraphConsistencyValidator::OrderGraphConsistencyValidator(
    vda5050pp::interface_agv::Handle &handle) noexcept(true)
    : Validator(k_description), handle_(handle) {}

std::list<vda5050pp::Error> OrderGraphConsistencyValidator::operator()(
    const vda5050pp::Order &order) const {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  std::set<uint32_t> found_base;
  std::set<uint32_t> found_horz;

  if (order.nodes.empty()) {
    return {{"EmptyOrder",
             {{{"order.orderId", order.orderId},
               {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
             "This order does not contain any nodes",
             ErrorLevel::WARNING}};
  }

  // Check node sequence ids
  for (auto &node : order.nodes) {
    if (node.sequenceId % 2 == 1) {
      return {{"InvalidNodeSequenceId",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)},
                 {"node.nodeId", node.nodeId},
                 {"node.sequenceId", std::to_string(node.sequenceId)}}},
               "The order contains a node with an odd sequence id",
               ErrorLevel::WARNING}};
    }
    if (found_base.find(node.sequenceId) != found_base.end() ||
        found_horz.find(node.sequenceId) != found_horz.end()) {
      return {{"DuplicateSequenceId",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)},
                 {"node.nodeId", node.nodeId},
                 {"node.sequenceId", std::to_string(node.sequenceId)}}},
               "The order contains duplicate sequence ids",
               ErrorLevel::WARNING}};
    }
    if (node.released) {
      found_base.insert(node.sequenceId);
    } else {
      found_horz.insert(node.sequenceId);
    }
  }

  // Check edge sequence ids
  for (auto &edge : order.edges) {
    if (edge.sequenceId % 2 == 0) {
      return {{"InvalidNodeSequenceId",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)},
                 {"edge.edgeId", edge.edgeId},
                 {"edge.sequenceId", std::to_string(edge.sequenceId)}}},
               "The order contains a edge with an even sequence id",
               ErrorLevel::WARNING}};
    }
    if (found_base.find(edge.sequenceId) != found_base.end() ||
        found_horz.find(edge.sequenceId) != found_horz.end()) {
      return {{"DuplicateSequenceId",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)},
                 {"edge.edgeId", edge.edgeId},
                 {"edge.sequenceId", std::to_string(edge.sequenceId)}}},
               "The order contains duplicate sequence ids",
               ErrorLevel::WARNING}};
    }
    if (edge.released) {
      found_base.insert(edge.sequenceId);
    } else {
      found_horz.insert(edge.sequenceId);
    }
  }

  // Check if there are no seqId skips
  auto max_seq = found_horz.empty() ? *found_base.crbegin()
                                    : std::max(*found_base.crbegin(), *found_horz.crbegin());
  auto min_seq = found_horz.empty() ? *found_base.cbegin()
                                    : std::min(*found_base.cbegin(), *found_horz.cbegin());
  if (max_seq - min_seq + 1 != found_base.size() + found_horz.size()) {
    return {{"MissingSequenceId",
             {{{"order.orderId", order.orderId},
               {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
             "The order skips sequence ids",
             ErrorLevel::WARNING}};
  }

  // Check if horizon && base are clearly separated
  if (!found_horz.empty()) {
    auto max_base_seq = *found_base.crbegin();
    auto min_horz_seq = *found_horz.cbegin();
    if (min_horz_seq - max_base_seq != 1) {
      return {{"HorizonSeqLTBaseSeq",
               {{{"order.orderId", order.orderId},
                 {"order.orderUpdateId", std::to_string(order.orderUpdateId)}}},
               "The order contains a horizon sequence id smaller then a base sequence id",
               ErrorLevel::WARNING}};
    }
  }

  return {};
}