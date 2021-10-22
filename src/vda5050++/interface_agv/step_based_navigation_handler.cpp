// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/step_based_navigation_handler.h"

#include <string>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/drive_to_node_manager.h"

using namespace vda5050pp::interface_agv;

void StepBasedNavigationHandler::setData(vda5050pp::Node &&goal_node,
                                         std::optional<vda5050pp::Edge> &&via_edge) noexcept(true) {
  this->goal_node_ = std::move(goal_node);
  this->via_edge_ = std::move(via_edge);
}

void StepBasedNavigationHandler::setManager(
    vda5050pp::core::logic::DriveToNodeManager &manager) noexcept(true) {
  this->manager_ = &manager;
}

void StepBasedNavigationHandler::setHandle(vda5050pp::interface_agv::Handle &handle) noexcept(
    true) {
  this->handle_ = &handle;
}

void StepBasedNavigationHandler::addError(const std::string &description,
                                          vda5050pp::ErrorLevel level) {
  this->addError(description, level, {});
}
void StepBasedNavigationHandler::addError(
    const std::string &description, vda5050pp::ErrorLevel level,
    const std::vector<vda5050pp::ErrorReference> &additional_references) {
  vda5050pp::Error error;
  error.errorDescription = description;
  error.errorLevel = level;
  error.errorType = "navigateToNodeError";
  error.errorReferences = {
      {"node.nodeId", this->getGoalNode().nodeId},
      {"node.sequenceId", std::to_string(this->getGoalNode().sequenceId)},
  };
  std::move(begin(additional_references), end(additional_references),
            std::back_inserter(*error.errorReferences));

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addError(error);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
}
void StepBasedNavigationHandler::addInfo(const std::string &description,
                                         vda5050pp::InfoLevel level) {
  this->addInfo(description, level, {});
}
void StepBasedNavigationHandler::addInfo(
    const std::string &description, vda5050pp::InfoLevel level,
    const std::vector<vda5050pp::InfoReference> &additional_references) {
  vda5050pp::Info info;
  info.infoDescription = description;
  info.infoLevel = level;
  info.infoType = "navigateToNodeInfo";
  info.infoReferences = {
      {"node.nodeId", this->getGoalNode().nodeId},
      {"node.sequenceId", std::to_string(this->getGoalNode().sequenceId)},
  };
  std::move(begin(additional_references), end(additional_references),
            std::back_inserter(*info.infoReferences));

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().addInfo(info);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void StepBasedNavigationHandler::failed() {
  if (this->manager_ != nullptr && !this->manager_->failed()) {
    throw std::logic_error("The AGV is not in a failable state");
  }
}
void StepBasedNavigationHandler::finished() {
  if (this->manager_ != nullptr && !this->manager_->finished()) {
    throw std::logic_error("The AGV is not in a finishable state");
  }
}
void StepBasedNavigationHandler::paused() {
  if (this->manager_ != nullptr && !this->manager_->paused()) {
    throw std::logic_error("The AGV is not in a pausable state");
  }
}
void StepBasedNavigationHandler::resumed() {
  if (this->manager_ != nullptr && !this->manager_->resumed()) {
    throw std::logic_error("The AGV is not in a resumable state");
  }
}

void StepBasedNavigationHandler::started() {
  if (this->manager_ != nullptr && !this->manager_->started()) {
    throw std::logic_error("The AGV is not in a startable state");
  }
}

void StepBasedNavigationHandler::setPosition(const vda5050pp::AGVPosition &position) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().setAGVPosition(position);
}
void StepBasedNavigationHandler::updateDistanceSinceLastNode(double distance) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  ha.getState().setDistanceSinceLastNode(distance);
}

const vda5050pp::Node &StepBasedNavigationHandler::getGoalNode() const { return this->goal_node_; }

const std::optional<vda5050pp::Edge> &StepBasedNavigationHandler::getViaEdge() const {
  return this->via_edge_;
}