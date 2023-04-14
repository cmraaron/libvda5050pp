// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/logic.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

Logic::Logic(vda5050pp::interface_agv::Handle &handle) noexcept(true)
    : net_manager_(handle), instant_actions_manager_(handle, net_manager_) {}

void Logic::notifyRunningActionsPause() noexcept(true) {
  this->net_manager_.pauseAllRunningActions();
}
void Logic::notifyRunningActionsResume() noexcept(true) {
  this->net_manager_.resumeAllPausedActions();
}
void Logic::notifyRunningNavigationPause() noexcept(true) { this->net_manager_.pauseDriving(); }
void Logic::notifyRunningNavigationResume() noexcept(true) { this->net_manager_.resumeDriving(); }

void Logic::doInstantAction(const vda5050pp::Action &instant_action) noexcept(false) {
  this->instant_actions_manager_.doInstantAction(instant_action);
}

void Logic::reStartLogic() noexcept(true) { this->net_manager_.tick(); }

void Logic::interpretOrder() noexcept(false) { this->net_manager_.interpret(); }

void Logic::clear() noexcept(true) { this->net_manager_.clear(); }

void Logic::abortOrder(const std::function<void()> &and_then) noexcept(true) {
  this->net_manager_.cancelAll();
  this->net_manager_.onAllExited(and_then);
}

void Logic::notifyHorizonChanged() noexcept(true) { this->net_manager_.notifyHorizonChanged(); }