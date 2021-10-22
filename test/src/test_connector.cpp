// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/test_connector.h"

test::TestConnector::~TestConnector() {}

void test::TestConnector::setConsumer(
    std::weak_ptr<vda5050pp::interface_mc::MessageConsumer>) noexcept(true) {}

void test::TestConnector::queueConnection(const vda5050pp::Connection &) noexcept(false) {}

void test::TestConnector::queueState(const vda5050pp::State &) noexcept(false) {}

void test::TestConnector::queueVisualization(const vda5050pp::Visualization &) noexcept(false) {}

void test::TestConnector::connect() noexcept(false) {}

void test::TestConnector::disconnect() noexcept(false) {}

void test::TestConnector::disconnect(const vda5050pp::Connection &) noexcept(false) {}