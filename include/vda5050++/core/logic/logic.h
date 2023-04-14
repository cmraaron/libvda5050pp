// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the Logic module class
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_LOGIC_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_LOGIC_HPP_

#include "vda5050++/core/logic/instant_actions_manager.h"
#include "vda5050++/core/logic/net_manager.h"
#include "vda5050++/model/Action.h"
#include "vda5050++/model/Order.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
/// \brief The main class of the logic module, providing access to logic functions
///
class Logic {
private:
  NetManager net_manager_;
  InstantActionsManager instant_actions_manager_;

public:
  explicit Logic(vda5050pp::interface_agv::Handle &handle) noexcept(true);

  ///
  ///\brief call onPause() for each running task
  ///
  void notifyRunningActionsPause() noexcept(true);

  ///
  ///\brief call onResume() for each paused task
  ///
  void notifyRunningActionsResume() noexcept(true);

  ///
  ///\brief call onPause() for each running task
  ///
  void notifyRunningNavigationPause() noexcept(true);

  ///
  ///\brief call onResume() for each paused task
  ///
  void notifyRunningNavigationResume() noexcept(true);

  ///
  ///\brief Execute an instant action
  ///
  ///\param instant_action
  ///
  void doInstantAction(const vda5050pp::Action &instant_action) noexcept(false);

  ///
  ///\brief Interpret any new order data in the state
  ///
  void interpretOrder() noexcept(false);

  ///
  /// \brief re/-start the logic. Used when resuming after an order was received (not started yet)
  ///
  /// Effectively deep-ticks every place of the PTN. Can be used to
  /// re enable the net when it is in a manual-fire-only state
  ///
  void reStartLogic() noexcept(true);

  ///
  /// \brief Clear the state of the logic module
  ///
  /// Reset the PetriNet and prepare for building a new one
  ///
  void clear() noexcept(true);

  ///
  ///\brief Skip all pending tasks, and finish running tasks normally
  ///
  ///\param and_then function to execute after order was aborted
  ///
  void abortOrder(const std::function<void()> &and_then = nullptr) noexcept(true);

  void notifyHorizonChanged() noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_LOGIC_HPP_
