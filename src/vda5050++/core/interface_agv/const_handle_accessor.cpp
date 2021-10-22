// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/interface_agv/const_handle_accessor.h"

using namespace vda5050pp::core::interface_agv;

ConstHandleAccessor::ConstHandleAccessor(const vda5050pp::interface_agv::Handle &handle)
    : handle_(handle) {}

const vda5050pp::interface_agv::Handle &ConstHandleAccessor::getHandle() const noexcept(true) {
  return this->handle_;
}

const vda5050pp::core::state::StateManager &ConstHandleAccessor::getState() const noexcept(true) {
  return this->handle_.state_manager_;
}

const std::shared_ptr<vda5050pp::interface_mc::Connector>
vda5050pp::core::interface_agv::ConstHandleAccessor::getConnector() const noexcept(true) {
  return this->handle_.connector_;
}

const vda5050pp::core::logic::Logic &ConstHandleAccessor::getLogic() const noexcept(true) {
  return this->handle_.logic_;
}

const vda5050pp::core::validation::ValidationProvider &ConstHandleAccessor::getValidationProvider()
    const noexcept(true) {
  return this->handle_.validation_provider_;
}

const vda5050pp::core::messages::Messages &ConstHandleAccessor::getMessages() const noexcept(true) {
  return this->handle_.messages_;
}

vda5050pp::interface_agv::Logger &vda5050pp::core::interface_agv::ConstHandleAccessor::getLogger()
    const noexcept(true) {
  return *vda5050pp::interface_agv::Logger::getCurrentLogger();  // TODO: change this awful
                                                                 // dereference
}

const vda5050pp::interface_agv::agv_description::AGVDescription &
ConstHandleAccessor::getAGVDescription() const noexcept(true) {
  return this->handle_.agv_description_;
}

std::chrono::system_clock::duration ConstHandleAccessor::getStateUpdatePeriod() const
    noexcept(true) {
  return this->handle_.state_update_period_;
}

bool ConstHandleAccessor::isContinuousNavigation() const noexcept(true) {
  return this->handle_.create_continuous_navigation_handler_ != nullptr;
}
bool ConstHandleAccessor::isStepBasedNavigation() const noexcept(true) {
  return this->handle_.create_navigate_to_node_handler_ != nullptr;
}