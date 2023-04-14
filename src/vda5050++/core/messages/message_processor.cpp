// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/messages/message_processor.h"

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

vda5050pp::core::messages::MessageProcessor::MessageProcessor(
    vda5050pp::interface_agv::Handle &handle)
    : handle_(handle) {}

void vda5050pp::core::messages::MessageProcessor::receivedConnection(
    const vda5050pp::Connection &connection) noexcept(true) {
  (void)connection;
}

void vda5050pp::core::messages::MessageProcessor::receivedInstantActions(
    const vda5050pp::InstantActions &instant_actions) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &logger = ha.getLogger();
  auto &state = ha.getState();
  auto &messages = ha.getMessages();
  auto &logic = ha.getLogic();
  auto &validationProvider = ha.getValidationProvider();

  logger.logInfo(vda5050pp::core::common::logstring("Received InstantAction #",
                                                    instant_actions.header.headerId));

  // BEGIN Critical section (Only one msg at a time can be validated and inserted into the state,
  //                         mainly because the validation of two contradicting messages
  //                         which aren't contradicting by themselves can't be prevented otherwise.
  //                         e.g. duplicate ids but different behaviour)
  {
    auto lock = std::lock_guard(this->ctrl_mutex_);

    // Report errors, if there are any
    if (auto errors = validationProvider.validateInstantActions(instant_actions); !errors.empty()) {
      logger.logWarn(vda5050pp::core::common::logstring("InstantAction #",
                                                        instant_actions.header.headerId,
                                                        "contained errors, and won't be executed"));

      for (const auto &e : errors) {
        state.addError(e);
      }

      messages.requestStateUpdate(UpdateUrgency::k_immediate);

      return;
    }
    state.insertInstantActions(instant_actions);
  }
  // END Critical section

  // Process  InstantActions
  for (const auto &action : instant_actions.instantActions) {
    logic.doInstantAction(action);
  }

  messages.requestStateUpdate(UpdateUrgency::k_immediate);
}

void vda5050pp::core::messages::MessageProcessor::receivedOrder(
    const vda5050pp::Order &order) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &logger = ha.getLogger();
  auto &state = ha.getState();
  auto &messages = ha.getMessages();
  auto &logic = ha.getLogic();
  auto &validationProvider = ha.getValidationProvider();

  logger.logInfo(vda5050pp::core::common::logstring(
      "Received Order #", order.header.headerId, " :: ", order.orderId, "@", order.orderUpdateId));

  // BEGIN Critical section (Only one msg at a time can be validated and inserted into the state,
  //                         mainly because the validation of two contradicting messages
  //                         which aren't contradicting by themselves can't be prevented otherwise.
  //                         e.g. duplicate ids but different behaviour)
  {
    auto lock = std::lock_guard(this->ctrl_mutex_);

    if (validationProvider.ignoreOrder(order)) {
      logger.logInfo(vda5050pp::core::common::logstring("Discarding duplicate Order ",
                                                        order.orderId, "@", order.orderUpdateId));
      return;
    }

    // Report errors, if there are any
    if (auto errors = validationProvider.validateOrder(order); !errors.empty()) {
      logger.logWarn(vda5050pp::core::common::logstring(
          "Order #", order.header.headerId,
          "contained errors, and won't be inserted into the state"));

      for (const auto &e : errors) {
        state.addError(e);
      }

      messages.requestStateUpdate(UpdateUrgency::k_immediate);

      return;
    }

    // check if the order will be overwritten || extended
    auto cmp_seq = [](auto &n1, auto &n2) -> bool { return n1.sequenceId < n2.sequenceId; };
    auto order_first_node_it = std::min_element(cbegin(order.nodes), cend(order.nodes), cmp_seq);

    bool appends = state.getGraphBaseSeqId() != 0 &&
                   state.getGraphBaseSeqId() == order_first_node_it->sequenceId;

    if ((state.isIdle() || order.orderId != state.getOrderId()) && !appends) {
      logic.clear();
      state.setOrder(order);
    } else {
      state.appendOrder(order);
    }
  }
  // END Critical section

  logic.interpretOrder();

  messages.requestStateUpdate(UpdateUrgency::k_immediate);
}