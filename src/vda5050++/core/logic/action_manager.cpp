// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/action_manager.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

ActionManager::ActionManager(vda5050pp::interface_agv::Handle &handle, vda5050pp::Action action,
                             SeqNrT seq)
    : TaskManager(handle, seq) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  this->action_handler_ = ha.createActionHandler();
  this->action_handler_->setAction(std::move(action));
  this->action_handler_->setHandleReference(handle);
  this->action_handler_->setManagerReference(*this);
}

std::shared_ptr<vda5050pp::interface_agv::ActionHandler> ActionManager::getHandler() const
    noexcept(true) {
  return this->action_handler_;
}

void ActionManager::taskInitialize() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();
  auto &action = this->action_handler_->getAction();
  auto &q = ha.getTaskQueue();
  auto &messages = ha.getMessages();

  state.setActionStatus(action.actionId, vda5050pp::ActionStatus::INITIALIZING);

  q.push([this] {
    try {
      this->action_handler_->start(this->action_handler_->getAction());
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(e, "ActionHandler::start()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getLogic().abortOrder();
    }
  });

  messages.requestStateUpdate(messages::UpdateUrgency::k_high);
}
void ActionManager::taskRunning() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();
  auto &action = this->action_handler_->getAction();

  state.setActionStatus(action.actionId, vda5050pp::ActionStatus::RUNNING);
  msgs.requestStateUpdate(messages::UpdateUrgency::k_medium);
}
void ActionManager::taskPaused() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();
  auto &action = this->action_handler_->getAction();

  state.setActionStatus(action.actionId, vda5050pp::ActionStatus::PAUSED);
  msgs.requestStateUpdate(messages::UpdateUrgency::k_medium);
}
void ActionManager::taskFinished() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();
  auto &action = this->action_handler_->getAction();

  state.setActionStatus(action.actionId, vda5050pp::ActionStatus::FINISHED);
  msgs.requestStateUpdate(messages::UpdateUrgency::k_high);
}
void ActionManager::taskFailed() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();
  auto &action = this->action_handler_->getAction();

  state.setActionStatus(action.actionId, vda5050pp::ActionStatus::FAILED);
  msgs.requestStateUpdate(messages::UpdateUrgency::k_high);
}

bool ActionManager::logTransition(const std::string &name, bool ret) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring(
          "ActionManager@", this->action_handler_->getAction().actionId, ": ", name, " -> ",
          ret ? "success" : "failure"));

  return ret;
}

void ActionManager::logPlaceReached(const Net::PlaceT &place) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring("ActionManager@",
                                                   this->action_handler_->getAction().actionId,
                                                   ": reached ", place.getID()));
}

void ActionManager::pause() noexcept(true) {
  auto &q = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getTaskQueue();
  q.push([this] {
    this->logTransition("pause()", true);
    try {
      this->action_handler_->pause(this->action_handler_->getAction());
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(e, "ActionHandler::pause()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
      ha.getLogic().abortOrder();
    }
  });
}

void ActionManager::resume() noexcept(true) {
  auto &q = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getTaskQueue();
  q.push([this] {
    this->logTransition("resume()", true);
    try {
      this->action_handler_->resume(this->action_handler_->getAction());
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error =
          vda5050pp::core::common::errorFromInternalException(e, "ActionHandler::resume()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
      ha.getLogic().abortOrder();
    }
  });
}

void ActionManager::stop() noexcept(true) {
  auto &q = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getTaskQueue();

  q.push([this] {
    this->logTransition("stop()", true);
    try {
      this->action_handler_->stop(this->action_handler_->getAction());
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(e, "ActionHandler::stop()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
      ha.getLogic().abortOrder();
    }
  });
}