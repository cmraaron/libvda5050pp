// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the Messages declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_MESSAGES_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_MESSAGES_HPP_

#include <memory>

#include "vda5050++/core/messages/message_processor.h"
#include "vda5050++/core/messages/state_update_timer.h"
#include "vda5050++/model/Visualization.h"

// Forward declarations, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::messages {

///
/// \brief This is the main class of the message module. It provides access to messaging
/// functions to other modules.
///
class Messages {
  friend class StateUpdateTimer;

private:
  vda5050pp::interface_agv::Handle &handle_;
  std::shared_ptr<MessageProcessor> message_processor_;
  StateUpdateTimer state_update_timer_;

  vda5050pp::Header mkHeader(uint32_t seq) const noexcept(true);

  void sendState() noexcept(true);

public:
  ///
  /// \brief Connect via the connector
  ///
  /// \param handle
  ///
  explicit Messages(vda5050pp::interface_agv::Handle &handle);

  Messages(const Messages &) = delete;
  Messages(Messages &&) = delete;

  ///
  /// \brief Disconnect the connector
  ///
  ~Messages();

  ///
  /// \brief Connect to mc
  ///
  void connect() noexcept(false);

  ///
  /// \brief Disconnect from mc
  ///
  void disconnect() noexcept(false);

  ///
  ///\brief Get the libraries messages processor instance
  ///
  ///\return MessageProcessor&
  ///
  MessageProcessor &getMessageProcessor() noexcept(true);

  ///
  /// \brief Request a state update with an urgency.
  ///        The urgency is used to possibly send a combined state update
  ///        when correlating events occur.
  ///
  /// \param urgency the urgency of the request
  ///
  void requestStateUpdate(UpdateUrgency urgency) noexcept(true);

  ///
  /// \brief Send a visualization msg (header will be filled in)
  ///
  /// \param visualization the visualization message
  ///
  void sendVisualization(const vda5050pp::Visualization &visualization) noexcept(false);
};

}  // namespace vda5050pp::core::messages

#endif  // INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_MESSAGES_HPP_
