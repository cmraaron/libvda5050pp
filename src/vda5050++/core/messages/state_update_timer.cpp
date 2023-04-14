// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the implementation of the state update timer
//

#include "vda5050++/core/messages/state_update_timer.h"

#include <ctime>
#include <functional>
#include <iomanip>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::messages;

void StateUpdateTimer::timerRoutine() {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  TimePointT wakeup_time_point;

  ha.getLogger().logDebug("StateUpdateTimer: starting...\n");

  while (this->active_) {
    wakeup_time_point = this->last_sent_ + ha.getStateUpdatePeriod();

    // If there was any request for an earlier update, use it.
    if (this->next_scheduled_update_.has_value()) {
      wakeup_time_point = std::min(wakeup_time_point, *this->next_scheduled_update_);
    }

#ifdef HAVE_CTIME_LOCALTIME_R
    auto c_time = std::chrono::system_clock::to_time_t(wakeup_time_point);
    struct std::tm tm_buf;  // Use this for the localtime_r result buffer
    ha.getLogger().logDebug(
        vda5050pp::core::common::logstring("StateUpdateTimer: Next state update planned for ",
                                           std::put_time(localtime_r(&c_time, &tm_buf), "%c %Z")));
#endif

    auto status = this->timer_.sleepUntil(wakeup_time_point);

    if (status == vda5050pp::core::common::InterruptableTimerStatus::k_ok) {
      // Timer was not interrupted, so there is no new update time point
      ha.getMessages().sendState();

      this->last_sent_ = std::chrono::system_clock::now();
      this->next_scheduled_update_.reset();
    } else {
      // Timer was interrupted, so there is a new update time point,
      // update the wakeup time point (in the next iteration) and go to sleep again.
    }
  }

  ha.getLogger().logDebug("StateUpdateTimer: exiting...\n");
}

StateUpdateTimer::StateUpdateTimer(vda5050pp::interface_agv::Handle &handle)
    : active_(true), handle_(handle) {
  this->last_sent_ = std::chrono::system_clock::now();
  auto this_timerRoutine = std::bind(std::mem_fn(&StateUpdateTimer::timerRoutine), this);
  this->thread_ = std::make_unique<std::thread>(this_timerRoutine);
}

StateUpdateTimer::~StateUpdateTimer() {
  this->active_ = false;
  this->timer_.disable();
  this->thread_->join();
  this->thread_.reset();
}

void StateUpdateTimer::requestUpdate(UpdateUrgency urgency) noexcept(true) {
  auto update_time_point = std::chrono::system_clock::now() + durationFromUpdateUrgency(urgency);

  if (this->next_scheduled_update_.has_value()) {
    update_time_point = std::min(update_time_point, *this->next_scheduled_update_);
  }

  if (urgency == UpdateUrgency::k_immediate) {
    // Immediate requires blocking until the state is actually sent
    // -> Send it synchronously
    auto &msgs = vda5050pp::core::interface_agv::HandleAccessor(this->handle_).getMessages();
    msgs.sendState();
    this->last_sent_ = std::chrono::system_clock::now();
    this->next_scheduled_update_.reset();
    this->timer_.interruptAll();  // cancel current sleep

  } else {
    // Set new update timepoint and interrupt
    this->next_scheduled_update_ = update_time_point;
    this->timer_.interruptAll();
  }
}