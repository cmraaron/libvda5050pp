// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the Connector interface class
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR
#define INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR

#include <memory>

#include "vda5050++/interface_mc/message_consumer.h"
#include "vda5050++/model/Connection.h"
#include "vda5050++/model/State.h"
#include "vda5050++/model/Visualization.h"

namespace vda5050pp::interface_mc {

///
///\brief Interface class for the MC connection
///
/// This should be implemented by the user in order to provide a connection to the MC.
/// NOTE: It is expected, that the specialized Connector runs internal threads
///       to receive the messages. Otherwise @see ConnectorPassive
///
class Connector {
public:
  virtual ~Connector() = default;

  ///
  ///\brief Set the consumer for the ingoing messages
  ///
  ///\param consumer the consumer
  ///
  virtual void setConsumer(
      std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) = 0;

  ///\brief Queue a connection message for sending
  virtual void queueConnection(const vda5050pp::Connection &connection) noexcept(false) = 0;

  /// \brief Queue a State message for sending
  virtual void queueState(const vda5050pp::State &state) noexcept(false) = 0;

  /// \brief Queue a Visualization message for sending
  virtual void queueVisualization(const vda5050pp::Visualization &visualization) noexcept(
      false) = 0;

  ///
  ///\brief Start the connection
  ///
  /// This can be used to manually initiate a connection
  ///
  virtual void connect() noexcept(false) = 0;

  ///
  ///\brief End the current connection, send offline msg
  ///
  virtual void disconnect() noexcept(false) = 0;
};

}  // namespace vda5050pp::interface_mc

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR */
