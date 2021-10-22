// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/pause_resume_handler.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/pause_resume_action_manager.h"
#include "vda5050++/model/Error.h"
#include "vda5050++/model/Info.h"

using namespace vda5050pp::interface_agv;

void PauseResumeHandler::setPauseOrResume(bool pause) noexcept(true) { this->is_pause_ = pause; }

void PauseResumeHandler::setHandleReference(vda5050pp::interface_agv::Handle &handle) noexcept(
    true) {
  this->handle_ = &handle;
}

void PauseResumeHandler::setManagerReference(
    vda5050pp::core::logic::PauseResumeActionManager &manager) noexcept(true) {
  this->manager_ = &manager;
}

void PauseResumeHandler::addError(const std::string &description, vda5050pp::ErrorLevel level) {
  this->addError(description, level, {});
}

void PauseResumeHandler::addError(
    const std::string &description, vda5050pp::ErrorLevel level,
    const std::vector<vda5050pp::ErrorReference> &additional_references) {
  vda5050pp::Error error;
  error.errorDescription = description;
  error.errorLevel = level;
  error.errorType = this->is_pause_ ? "pauseError" : "resumeError";
  error.errorReferences = additional_references;

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addError(error);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
}

void PauseResumeHandler::addInfo(const std::string &description, vda5050pp::InfoLevel level) {
  this->addInfo(description, level, {});
}

void PauseResumeHandler::addInfo(
    const std::string &description, vda5050pp::InfoLevel level,
    const std::vector<vda5050pp::InfoReference> &additional_references) {
  vda5050pp::Info info;
  info.infoDescription = description;
  info.infoLevel = level;
  info.infoType = this->is_pause_ ? "pauseError" : "resumeError";
  info.infoReferences = additional_references;

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addInfo(info);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void PauseResumeHandler::failed() {
  if (this->manager_ != nullptr) {
    this->manager_->failed();
  }
}

void PauseResumeHandler::finished() {
  if (this->manager_ != nullptr) {
    this->manager_->finished();
  }
}

void PauseResumeHandler::notifyActions() {
  if (this->manager_ != nullptr) {
    this->manager_->notifyRunningActions();
  }
}

void PauseResumeHandler::notifyNavigation() {
  if (this->manager_ != nullptr) {
    this->manager_->notifyRunningNavigation();
  }
}