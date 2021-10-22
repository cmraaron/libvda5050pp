// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/pause_resume_action_manager.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

PauseResumeActionManager::PauseResumeActionManager(vda5050pp::interface_agv::Handle &handle,
                                                   const vda5050pp::Action &action)
    : handle_(handle), action_(action) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  this->pause_resume_handler_ = ha.createPauseResumeHandler();
  this->pause_resume_handler_->setManagerReference(*this);
  this->pause_resume_handler_->setHandleReference(handle);
  this->pause_resume_handler_->setPauseOrResume(action.actionType == "startPause");
}

void PauseResumeActionManager::initialize() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  q.push([this] {
    try {
      if (this->pause_resume_handler_->is_pause_) {
        this->pause_resume_handler_->doPause();
      } else {
        this->pause_resume_handler_->doResume();
      }
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, this->pause_resume_handler_->is_pause_ ? "PauseResumeHandler::pause()"
                                                    : "PauseResumeHandler::resume()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
      ha.getLogic().abortOrder();
    }
  });
}

void PauseResumeActionManager::started() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();
  auto &messages = ha.getMessages();

  state.setActionStatus(this->action_.actionId, vda5050pp::ActionStatus::RUNNING);

  messages.requestStateUpdate(messages::UpdateUrgency::k_medium);
}

void PauseResumeActionManager::failed() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();
  auto &messages = ha.getMessages();

  state.setActionStatus(this->action_.actionId, vda5050pp::ActionStatus::FAILED);

  messages.requestStateUpdate(messages::UpdateUrgency::k_high);
}

void PauseResumeActionManager::finished() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();
  auto &messages = ha.getMessages();

  state.setActionStatus(this->action_.actionId, vda5050pp::ActionStatus::FINISHED);

  state.setPausedState(this->pause_resume_handler_->is_pause_);

  messages.requestStateUpdate(messages::UpdateUrgency::k_high);
}

void PauseResumeActionManager::notifyRunningActions() noexcept(true) {
  auto &logic = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getLogic();

  if (this->pause_resume_handler_->is_pause_) {
    logic.notifyRunningActionsPause();
  } else {
    logic.notifyRunningActionsResume();
    logic.reStartLogic();
  }
}

void PauseResumeActionManager::notifyRunningNavigation() noexcept(true) {
  auto &logic = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getLogic();

  if (this->pause_resume_handler_->is_pause_) {
    logic.notifyRunningNavigationPause();
  } else {
    logic.notifyRunningNavigationResume();
    logic.reStartLogic();
  }
}