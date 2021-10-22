// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// TODO: Short description
//

#ifndef TEST_INCLUDE_TEST_TEST_CONNECTOR
#define TEST_INCLUDE_TEST_TEST_CONNECTOR

#include <optional>

#include "vda5050++/interface_mc/connector.h"

namespace test {

class TestConnector : public vda5050pp::interface_mc::Connector {
public:
  virtual ~TestConnector();

  virtual void setConsumer(
      std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) override;

  virtual void queueConnection(const vda5050pp::Connection &connection) noexcept(false) override;

  virtual void queueState(const vda5050pp::State &state) noexcept(false) override;

  virtual void queueVisualization(const vda5050pp::Visualization &visualization) noexcept(
      false) override;

  virtual void connect() noexcept(false) override;

  virtual void disconnect() noexcept(false) override;

  virtual void disconnect(const vda5050pp::Connection &connection) noexcept(false) override;
};

}  // namespace test

#endif /* TEST_INCLUDE_TEST_TEST_CONNECTOR */
