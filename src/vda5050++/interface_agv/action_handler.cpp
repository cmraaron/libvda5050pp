// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/action_handler.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::interface_agv;

void ActionHandler::setAction(vda5050pp::Action &&action) noexcept(true) {
  this->action_ = std::move(action);
}

void ActionHandler::setHandleReference(vda5050pp::interface_agv::Handle &handle) noexcept(true) {
  this->handle_ = &handle;
}
void ActionHandler::setManagerReference(vda5050pp::core::logic::ActionManager &manager) noexcept(
    true) {
  this->action_manager_ = &manager;
}

void ActionHandler::addError(const std::string &description, vda5050pp::ErrorLevel level) {
  this->addError(description, level, {});
}

void ActionHandler::addError(const std::string &description, vda5050pp::ErrorLevel level,
                             const std::vector<vda5050pp::ErrorReference> &additional_references) {
  vda5050pp::Error error;
  error.errorDescription = description;
  error.errorLevel = level;
  error.errorType = "actionError";
  error.errorReferences = {
      {"action.actionId", this->action_.actionId},
      {"action.actionType", this->action_.actionType},
  };
  std::move(begin(additional_references), end(additional_references),
            std::back_inserter(*error.errorReferences));

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addError(error);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
}

void ActionHandler::addInfo(const std::string &description, vda5050pp::InfoLevel level) {
  this->addInfo(description, level, {});
}

void ActionHandler::addInfo(const std::string &description, vda5050pp::InfoLevel level,
                            const std::vector<vda5050pp::InfoReference> &additional_references) {
  vda5050pp::Info info;
  info.infoDescription = description;
  info.infoLevel = level;
  info.infoType = "actionInfo";
  info.infoReferences = {
      {"action.actionId", this->action_.actionId},
      {"action.actionType", this->action_.actionType},
  };
  std::move(begin(additional_references), end(additional_references),
            std::back_inserter(*info.infoReferences));

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addInfo(info);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void ActionHandler::failed() {
  if (this->action_manager_ != nullptr && !this->action_manager_->failed()) {
    throw std::logic_error("The Action is not in a failable state");
  }
}
void ActionHandler::finished() {
  if (this->action_manager_ != nullptr && !this->action_manager_->finished()) {
    throw std::logic_error("The Action is not in a finishable state");
  }
}
void ActionHandler::started() {
  if (this->action_manager_ != nullptr && !this->action_manager_->started()) {
    throw std::logic_error("The Action is not in a startable state");
  }
}
void ActionHandler::paused() {
  if (this->action_manager_ != nullptr && !this->action_manager_->paused()) {
    throw std::logic_error("The Action is not in a pausable state");
  }
}
void ActionHandler::resumed() {
  if (this->action_manager_ != nullptr && !this->action_manager_->resumed()) {
    throw std::logic_error("The Action is not in a resumable state");
  }
}
void ActionHandler::setResult(const std::string &result) {
  if (this->handle_ != nullptr) {
    vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
    ha.getState().setActionResult(this->getAction().actionId, result);
  }
}

const vda5050pp::Action &ActionHandler::getAction() const { return this->action_; }