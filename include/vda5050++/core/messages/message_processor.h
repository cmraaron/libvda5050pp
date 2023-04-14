// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the MessageProcessor declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_MESSAGE_PROCESSOR_MESSAGE_PROCESSOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_MESSAGE_PROCESSOR_MESSAGE_PROCESSOR_HPP_

#include <mutex>

#include "vda5050++/interface_mc/message_consumer.h"

// Forward declaration, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::messages {

///
/// \brief This class is MessageConsumer of this library. All received messages will be
/// processed by the instance of this class
///
class MessageProcessor : public vda5050pp::interface_mc::MessageConsumer {
private:
  vda5050pp::interface_agv::Handle &handle_;
  std::mutex ctrl_mutex_;

public:
  explicit MessageProcessor(vda5050pp::interface_agv::Handle &handle);

  ///
  ///\brief Received a Message in the connection topic
  ///
  ///\param connection  the Message
  ///
  virtual void receivedConnection(const vda5050pp::Connection &connection) noexcept(true) override;

  ///
  ///\brief Received a Message in the instant_actions topic
  ///
  ///\param instant_actions  the Message
  ///
  virtual void receivedInstantActions(const vda5050pp::InstantActions &instant_actions) noexcept(
      true) override;

  ///
  ///\brief Received a Message in the Order topic
  ///
  ///\param order  the Message
  ///
  virtual void receivedOrder(const vda5050pp::Order &order) noexcept(true) override;
};

}  // namespace vda5050pp::core::messages

#endif  // INCLUDE_VDA5050_2B_2B_CORE_MESSAGE_PROCESSOR_MESSAGE_PROCESSOR_HPP_
