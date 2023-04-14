// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the PauseResumeActionManager declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PAUSE_RESUME_ACTION_MANAGER
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PAUSE_RESUME_ACTION_MANAGER

#include <memory>

#include "../../interface_agv/pause_resume_handler.h"
#include "../../model/Action.h"

namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
/// \brief Manages a pause or resume instant action
///
class PauseResumeActionManager {
private:
  vda5050pp::interface_agv::Handle &handle_;
  vda5050pp::Action action_;
  std::shared_ptr<vda5050pp::interface_agv::PauseResumeHandler> pause_resume_handler_;

public:
  PauseResumeActionManager(vda5050pp::interface_agv::Handle &handle,
                           const vda5050pp::Action &action);

  ///
  /// \brief initialize (call PauseResumeActionCallback)
  ///
  void initialize() noexcept(true);

  ///
  /// \brief action is running
  ///
  void started() noexcept(true);

  ///
  /// \brief cannot pause/resume
  ///
  void failed() noexcept(true);

  ///
  /// \brief finished pause/resume -> sets paused state
  ///
  void finished() noexcept(true);

  ///
  /// \brief notify active actions handlers about a pause or resume event
  ///
  void notifyRunningActions() noexcept(true);

  ///
  /// \brief notify active navigation handlers about a pause or resume event
  ///
  void notifyRunningNavigation() noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PAUSE_RESUME_ACTION_MANAGER */
