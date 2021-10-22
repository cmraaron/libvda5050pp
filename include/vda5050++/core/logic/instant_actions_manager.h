// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the instantActionsManager
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_INSTANT_ACTIONS_MANAGER_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_INSTANT_ACTIONS_MANAGER_HPP_

#include <memory>

#include "vda5050++/core/logic/net_manager.h"
#include "vda5050++/core/logic/pause_resume_action_manager.h"

namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
///\brief This Manager manages all current instant actions
///
class InstantActionsManager {
private:
  std::unique_ptr<PauseResumeActionManager> pause_resume_action_manager_;
  vda5050pp::interface_agv::Handle &handle_;
  NetManager &net_manager_;

public:
  InstantActionsManager(vda5050pp::interface_agv::Handle &handle,
                        NetManager &net_manager) noexcept(true);

  ///
  ///\brief Manage the execution of an instant action
  ///
  ///\param action the instant action to execute
  ///
  void doInstantAction(const vda5050pp::Action &action) noexcept(false);
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_INSTANT_ACTIONS_MANAGER_HPP_
