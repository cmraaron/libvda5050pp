// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/test_connector.h"

test::TestConnector::~TestConnector() {}
void test::TestConnector::setConsumer(
    std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) {
  this->consumer_ = consumer;
}

void test::TestConnector::queueConnection(const vda5050pp::Connection &) noexcept(false) {}

void test::TestConnector::queueState(const vda5050pp::State &) noexcept(false) {}

void test::TestConnector::queueVisualization(const vda5050pp::Visualization &) noexcept(false) {}

void test::TestConnector::connect() noexcept(false) {}

void test::TestConnector::disconnect() noexcept(false) {}

void test::TestConnector::disconnect(const vda5050pp::Connection &) noexcept(false) {}

void test::TestConnector::receiveOrder(const vda5050pp::Order &order) noexcept(true) {
  auto ptr = this->consumer_.lock();
  if (ptr != nullptr) {
    ptr->receivedOrder(order);
  }
}

void test::TestConnector::receiveInstantActions(
    const vda5050pp::InstantActions &instant_actions) noexcept(true) {
  auto ptr = this->consumer_.lock();
  if (ptr != nullptr) {
    ptr->receivedInstantActions(instant_actions);
  }
}
