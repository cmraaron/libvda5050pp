// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/instant_actions_manager.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

InstantActionsManager::InstantActionsManager(vda5050pp::interface_agv::Handle &handle,
                                             NetManager &net_manager) noexcept(true)
    : handle_(handle), net_manager_(net_manager) {}

void InstantActionsManager::doInstantAction(const vda5050pp::Action &action) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  if (action.actionType == "stopPause" || action.actionType == "startPause") {
    // Use PauseResumeActionManager to take care of the action
    this->pause_resume_action_manager_ =
        std::make_unique<PauseResumeActionManager>(this->handle_, action);
    this->pause_resume_action_manager_->initialize();
  } else if (action.actionType == "cancelOrder") {
    if (!this->net_manager_.isAnythingActive()) {
      // There is nothing to cancel
      ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FAILED);

      vda5050pp::Error error;
      error.errorType = "noOrderToCancel";
      error.errorDescription = "There is nothing to cancel";
      error.errorLevel = ErrorLevel::WARNING;
      error.errorReferences = {{"action.actionId", action.actionId}};
      ha.getState().addError(error);

      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_high);
      return;
    }

    // Cancel Order will skip all pending actions and cancel the running ones.
    // Finish action when the tail of the order was reached.
    this->net_manager_.onAllExited([this, action] {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FINISHED);
      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_immediate);
      ha.getState().clearOrder();
      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_low);
      this->net_manager_.onAllExited(nullptr);
    });

    ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::RUNNING);
    ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_medium);
    this->net_manager_.cancelAll();

  } else if (action.actionType == "stateRequest") {
    // The message module will send a state update anyways, just set this action to finished
    ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FINISHED);
    // TODO: maybe ask the AGV for additional info
  } else {
    // default dispatch
    this->net_manager_.interceptWithAction(action);
  }
}