// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the MessageConsumer interface class
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_MC_MESSAGE_CONSUMER_HPP_
#define INCLUDE_VDA5050_2B_2B_INTERFACE_MC_MESSAGE_CONSUMER_HPP_

#include "vda5050++/model/Connection.h"
#include "vda5050++/model/InstantActions.h"
#include "vda5050++/model/Order.h"

namespace vda5050pp::interface_mc {

///
///\brief An interface class implemented by the library to accept
/// messages from the connector
///
class MessageConsumer {
public:
  virtual ~MessageConsumer() = default;

  ///
  ///\brief Received a Message in the connection topic
  ///
  ///\param connection  the Message
  ///
  virtual void receivedConnection(const vda5050pp::Connection &connection) noexcept(true) = 0;

  ///
  ///\brief Received a Message in the instant_actions topic
  ///
  ///\param instant_actions  the Message
  ///
  virtual void receivedInstantActions(const vda5050pp::InstantActions &instant_actions) noexcept(
      true) = 0;

  ///
  ///\brief Received a Message in the Order topic
  ///
  ///\param order  the Message
  ///
  virtual void receivedOrder(const vda5050pp::Order &order) noexcept(true) = 0;
};

}  // namespace vda5050pp::interface_mc

#endif  // INCLUDE_VDA5050_2B_2B_INTERFACE_MC_MESSAGE_CONSUMER_HPP_
