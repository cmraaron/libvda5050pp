// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/drive_to_node_manager.h"

#include <memory>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

bool DriveToNodeManager::logTransition(const std::string &name, bool ret) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring(
          "DrivingManager@Seq=", this->navigate_to_node_handler_->getGoalNode().sequenceId, ": ",
          name, " -> ", ret ? "success" : "failure"));

  return ret;
}

void DriveToNodeManager::logPlaceReached(const Net::PlaceT &place) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring(
          "DrivingManager@Seq=", this->navigate_to_node_handler_->getGoalNode().sequenceId,
          ": reached ", place.getID()));
}

void DriveToNodeManager::taskInitialize() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();

  q.push([this] {
    try {
      this->navigate_to_node_handler_->start(this->navigate_to_node_handler_->getViaEdge(),
                                             this->navigate_to_node_handler_->getGoalNode());
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "SingleStepNavigationHandler::start()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
    }
  });
}

void DriveToNodeManager::taskRunning() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();

  state.setDriving(true);
  if (this->on_driving_changed_ != nullptr) {
    this->on_driving_changed_(true);
  }

  msgs.requestStateUpdate(messages::UpdateUrgency::k_high);
}

void DriveToNodeManager::taskPaused() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();

  state.setDriving(false);
  if (this->on_driving_changed_ != nullptr) {
    this->on_driving_changed_(false);
  }

  msgs.requestStateUpdate(messages::UpdateUrgency::k_medium);
}

void DriveToNodeManager::taskFinished() noexcept(true) {
  auto &state = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getState();
  auto &messages = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();

  state.setDriving(false);
  if (this->on_driving_changed_ != nullptr) {
    this->on_driving_changed_(false);
  }

  state.setLastNodeReached(this->navigate_to_node_handler_->getGoalNode().sequenceId);

  messages.requestStateUpdate(messages::UpdateUrgency::k_high);
}

void DriveToNodeManager::taskFailed() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();
  auto &messages = ha.getMessages();
  auto &logic = ha.getLogic();

  state.setDriving(false);
  if (this->on_driving_changed_ != nullptr) {
    this->on_driving_changed_(false);
  }

  logic.abortOrder();
  messages.requestStateUpdate(messages::UpdateUrgency::k_high);
}

DriveToNodeManager::DriveToNodeManager(vda5050pp::interface_agv::Handle &handle,
                                       std::optional<vda5050pp::Edge> via_edge,
                                       vda5050pp::Node goal_node, SeqNrT seq)
    : TaskManager(handle, seq) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  this->navigate_to_node_handler_ = ha.createStepBasedNavigationHandler();
  this->navigate_to_node_handler_->setData(std::move(goal_node), std::move(via_edge));
  this->navigate_to_node_handler_->setHandle(this->handle_);
  this->navigate_to_node_handler_->setManager(*this);
}

void DriveToNodeManager::pause() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  q.push([this] {
    try {
      this->navigate_to_node_handler_->pause();
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "SingleStepNavigationHandler::pause()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
    }
  });
}

void DriveToNodeManager::resume() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  q.push([this] {
    try {
      this->navigate_to_node_handler_->resume();
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "SingleStepNavigationHandler::resume()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
    }
  });
}

void DriveToNodeManager::stop() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  q.push([this] {
    try {
      this->navigate_to_node_handler_->stop();
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "SingleStepNavigationHandler::stop()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
    }
  });
}

void DriveToNodeManager::onDrivingChanged(const std::function<void(bool driving)> &fn) noexcept(
    true) {
  this->on_driving_changed_ = fn;
}
