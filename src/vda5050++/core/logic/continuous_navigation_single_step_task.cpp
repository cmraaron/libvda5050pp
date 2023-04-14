// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/continuous_navigation_single_step_task.h"

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

ContinuousNavigationSingleStepTask::ContinuousNavigationSingleStepTask(
    vda5050pp::interface_agv::Handle &handle, ContinuousNavigationManager &manager,
    uint32_t node_seq_id, SeqNrT seq)
    : TaskManager(handle, seq), manager_(manager), node_seq_(node_seq_id) {}

bool ContinuousNavigationSingleStepTask::logTransition(const std::string &name,
                                                       bool ret) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring("ContinuousNavigationStep@Seq=", this->getSeq(),
                                                   ": ", name, " -> ",
                                                   ret ? "success" : "failure"));

  return ret;
}

void ContinuousNavigationSingleStepTask::logPlaceReached(const Net::PlaceT &place) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
      .getLogger()
      .logDebug(vda5050pp::core::common::logstring("ContinuousNavigationStep@Seq=", this->node_seq_,
                                                   ": reached ", place.getID()));
}

void ContinuousNavigationSingleStepTask::taskInitialize() noexcept(true) {
  this->manager_.stepEntered(*this);
}

void ContinuousNavigationSingleStepTask::taskRunning() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getState().setDriving(true);
  this->manager_.stepDrivingChanged(true);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void ContinuousNavigationSingleStepTask::taskPaused() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getState().setDriving(false);
  this->manager_.stepDrivingChanged(false);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void ContinuousNavigationSingleStepTask::taskFinished() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getState().setLastNodeReached(this->getNodeSequence());
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
  this->manager_.stepExited(*this);
}

void ContinuousNavigationSingleStepTask::taskFailed() noexcept(true) {
  this->manager_.stepExited(*this);
}

void ContinuousNavigationSingleStepTask::pause() noexcept(true) {
  // ignore
}
void ContinuousNavigationSingleStepTask::resume() noexcept(true) {
  // ignore
}
void ContinuousNavigationSingleStepTask::stop() noexcept(true) {
  // ignore
}

uint32_t ContinuousNavigationSingleStepTask::getNodeSequence() const noexcept(true) {
  return this->node_seq_;
}