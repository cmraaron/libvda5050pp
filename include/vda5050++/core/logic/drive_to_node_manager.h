// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the DriveToNodeManager declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DRIVE_TO_NODE_MANAGER_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DRIVE_TO_NODE_MANAGER_HPP_

#include <functional>

#include "vda5050++/core/logic/task_manager.h"
#include "vda5050++/interface_agv/step_based_navigation_handler.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
/// \brief Specialization of the TaskManager for Driving tasks
///
class DriveToNodeManager : public TaskManager {
private:
  std::shared_ptr<vda5050pp::interface_agv::StepBasedNavigationHandler> navigate_to_node_handler_;

  virtual bool logTransition(const std::string &name, bool ret) noexcept(true) override;

  virtual void logPlaceReached(const Net::PlaceT &place) noexcept(true) override;

  virtual void taskInitialize() noexcept(true) override;

  virtual void taskRunning() noexcept(true) override;

  virtual void taskPaused() noexcept(true) override;

  virtual void taskFinished() noexcept(true) override;

  virtual void taskFailed() noexcept(true) override;

  std::function<void(bool)> on_driving_changed_;

public:
  ///
  ///\brief Create a new DriveToNodeManager which owns an DriveToNodeHandle and can be attached
  /// to the globale PTN
  ///
  ///\param handle the library handle
  ///\param via_edge the edge leading to goal_node
  ///\param goal_node the goal node of this task
  ///\param seq the sequence number of this task
  ///
  DriveToNodeManager(vda5050pp::interface_agv::Handle &handle,
                     std::optional<vda5050pp::Edge> via_edge, vda5050pp::Node goal_node,
                     SeqNrT seq);

  ///
  ///\brief Pause the associated driving task (forward to handle.onPause)
  ///
  virtual void pause() noexcept(true) override;

  ///
  ///\brief Resume the associated driving task (forward to handle.onResume)
  ///
  virtual void resume() noexcept(true) override;

  ///
  ///\brief Stop the associated driving task
  ///
  virtual void stop() noexcept(true) override;

  ///
  ///\brief Call a function when this task starts/pauses/resumes/stops running
  ///
  ///\param fn the function to call
  ///
  void onDrivingChanged(const std::function<void(bool driving)> &fn) noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DRIVE_TO_NODE_MANAGER_HPP_
