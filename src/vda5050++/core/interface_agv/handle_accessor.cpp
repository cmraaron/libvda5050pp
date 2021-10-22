// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::interface_agv;

HandleAccessor::HandleAccessor(vda5050pp::interface_agv::Handle &handle) : handle_(handle) {}

const vda5050pp::interface_agv::Handle &HandleAccessor::getHandle() const noexcept(true) {
  return this->handle_;
}
vda5050pp::interface_agv::Handle &HandleAccessor::getHandle() noexcept(true) {
  return this->handle_;
}

const vda5050pp::core::state::StateManager &HandleAccessor::getState() const noexcept(true) {
  return this->handle_.state_manager_;
}
vda5050pp::core::state::StateManager &HandleAccessor::getState() noexcept(true) {
  return this->handle_.state_manager_;
}

const std::shared_ptr<vda5050pp::interface_mc::Connector>
vda5050pp::core::interface_agv::HandleAccessor::getConnector() const noexcept(true) {
  return this->handle_.connector_;
}

std::shared_ptr<vda5050pp::interface_mc::Connector>
vda5050pp::core::interface_agv::HandleAccessor::getConnector() noexcept(true) {
  return this->handle_.connector_;
}

const vda5050pp::core::logic::Logic &HandleAccessor::getLogic() const noexcept(true) {
  return this->handle_.logic_;
}

vda5050pp::core::logic::Logic &HandleAccessor::getLogic() noexcept(true) {
  return this->handle_.logic_;
}

const vda5050pp::core::validation::ValidationProvider &HandleAccessor::getValidationProvider() const
    noexcept(true) {
  return this->handle_.validation_provider_;
}

vda5050pp::core::validation::ValidationProvider &HandleAccessor::getValidationProvider() noexcept(
    true) {
  return this->handle_.validation_provider_;
}

const vda5050pp::core::messages::Messages &HandleAccessor::getMessages() const noexcept(true) {
  return this->handle_.messages_;
}

vda5050pp::core::messages::Messages &HandleAccessor::getMessages() noexcept(true) {
  return this->handle_.messages_;
}

vda5050pp::interface_agv::Logger &vda5050pp::core::interface_agv::HandleAccessor::getLogger() const
    noexcept(true) {
  return *vda5050pp::interface_agv::Logger::getCurrentLogger();  // TODO: change this awful
                                                                 // dereference
}

vda5050pp::core::common::BlockingQueue<std::function<void()>>
    &HandleAccessor::getTaskQueue() noexcept(true) {
  return this->handle_.task_queue_;
}

std::shared_ptr<vda5050pp::interface_agv::ActionHandler> HandleAccessor::createActionHandler() const
    noexcept(true) {
  if (this->handle_.create_action_handler_ == nullptr) {
    return nullptr;
  }

  return this->handle_.create_action_handler_();
}
std::shared_ptr<vda5050pp::interface_agv::StepBasedNavigationHandler>
HandleAccessor::createStepBasedNavigationHandler() const noexcept(true) {
  if (this->handle_.create_navigate_to_node_handler_ == nullptr) {
    return nullptr;
  }

  return this->handle_.create_navigate_to_node_handler_();
}

std::shared_ptr<vda5050pp::interface_agv::PauseResumeHandler>
HandleAccessor::createPauseResumeHandler() const noexcept(true) {
  if (this->handle_.create_pause_resume_handler_ == nullptr) {
    return nullptr;
  }

  return this->handle_.create_pause_resume_handler_();
}

std::shared_ptr<vda5050pp::interface_agv::ContinuousNavigationHandler>
HandleAccessor::createContinuousNavigationHandler() const noexcept(true) {
  if (this->handle_.create_continuous_navigation_handler_ == nullptr) {
    return nullptr;
  }

  return this->handle_.create_continuous_navigation_handler_();
}

bool HandleAccessor::isContinuousNavigation() const noexcept(true) {
  return this->handle_.create_continuous_navigation_handler_ != nullptr;
}

bool HandleAccessor::isStepBasedNavigation() const noexcept(true) {
  return this->handle_.create_navigate_to_node_handler_ != nullptr;
}

const vda5050pp::interface_agv::agv_description::AGVDescription &HandleAccessor::getAGVDescription()
    const noexcept(true) {
  return this->handle_.agv_description_;
}

std::chrono::system_clock::duration HandleAccessor::getStateUpdatePeriod() const noexcept(true) {
  return this->handle_.state_update_period_;
}