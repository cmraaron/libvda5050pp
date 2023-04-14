// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/messages/messages.h"

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/version.h"

using namespace vda5050pp::core::messages;

Messages::Messages(vda5050pp::interface_agv::Handle &handle)
    : handle_(handle),
      message_processor_(std::make_shared<MessageProcessor>(handle_)),
      state_update_timer_(handle_) {}

Messages::~Messages() { this->disconnect(); }

void Messages::connect() noexcept(false) {
  /* Link the message processor with the connector provided to the library */
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  if (ha.getConnector() == nullptr) {
    ha.getLogger().logError("No connector loaded!\n");
    return;
  }

  ha.getConnector()->setConsumer(this->message_processor_);

  ha.getLogger().logInfo("Connecting...\n");
  try {
    ha.getConnector()->connect();
  } catch (std::exception &e) {
    ha.getLogger().logError(
        vda5050pp::core::common::logstring("Connection failed: ", e.what(), "\n"));
    throw e;
  }
  ha.getLogger().logInfo("Connected!\n");
}

void Messages::disconnect() noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  // Send offline message and disconnect
  if (ha.getConnector() != nullptr) {
    ha.getConnector()->disconnect();
  }
}

vda5050pp::Header Messages::mkHeader(uint32_t seq) const noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  vda5050pp::Header header;
  header.headerId = seq;
  header.timestamp = std::chrono::system_clock::now();
  header.version = vda5050pp::core::version::current;
  header.manufacturer = ha.getAGVDescription().manufacturer;
  header.serialNumber = ha.getAGVDescription().serial_number;

  return header;
}

MessageProcessor &Messages::getMessageProcessor() noexcept(true) {
  return *this->message_processor_;
}

void Messages::sendState() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  auto state = ha.getState().dumpState();
  state.header = this->mkHeader(ha.getState().nextStateSeq());

  auto connector = ha.getConnector();
  if (connector == nullptr) {
    ha.getLogger().logError(vda5050pp::core::common::logstring(
        "Could not send State #", state.header.headerId, " no connector set"));
    return;
  }

  try {
    connector->queueState(state);
    ha.getLogger().logInfo(
        vda5050pp::core::common::logstring("State #", state.header.headerId, " sent"));
  } catch (const std::exception &e) {
    ha.getLogger().logError(vda5050pp::core::common::logstring(
        "Could not send State #", state.header.headerId, " due to exception: ", e.what()));
  }
}

void Messages::requestStateUpdate(UpdateUrgency urgency) noexcept(true) {
  this->state_update_timer_.requestUpdate(urgency);
}

void Messages::sendVisualization(const vda5050pp::Visualization &visualization) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto connector = ha.getConnector();
  auto v = visualization;
  v.header = this->mkHeader(ha.getState().nextVisualizationSeq());
  try {
    connector->queueVisualization(v);
    ha.getLogger().logInfo(
        vda5050pp::core::common::logstring("Visualization #", v.header.headerId, " sent"));
  } catch (const std::exception &e) {
    ha.getLogger().logError(vda5050pp::core::common::logstring(
        "Could not send Visualization #", v.header.headerId, " due to exception: ", e.what()));
  }
}