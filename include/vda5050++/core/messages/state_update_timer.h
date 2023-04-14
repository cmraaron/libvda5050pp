// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the declaration of the StateUpdateTimer
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_STATE_UPDATE_TIMER
#define INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_STATE_UPDATE_TIMER

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include "../common/interruptable_timer.h"
#include "update_urgency.h"

namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::messages {

///
/// \brief The StateUpdateTimer Class has a thread, that periodically sends a new state.
/// Upon requests this period might be decreased.
///
class StateUpdateTimer {
private:
  using TimePointT = std::chrono::system_clock::time_point;
  using DurationT = std::chrono::system_clock::duration;

  std::unique_ptr<std::thread> thread_;
  TimePointT last_sent_;
  std::optional<TimePointT> next_scheduled_update_;
  vda5050pp::core::common::InterruptableTimer timer_;

  bool active_;

  vda5050pp::interface_agv::Handle &handle_;

  void timerRoutine();

public:
  explicit StateUpdateTimer(vda5050pp::interface_agv::Handle &handle);

  StateUpdateTimer(const StateUpdateTimer &) = delete;
  StateUpdateTimer(StateUpdateTimer &&) = delete;

  ~StateUpdateTimer();

  ///
  /// \brief Request an update. The next update time point might be set to be sooner.
  ///
  /// \param urgency the urgency of the request
  ///
  void requestUpdate(UpdateUrgency urgency = UpdateUrgency::k_none) noexcept(true);
};

}  // namespace vda5050pp::core::messages

#endif /* INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_STATE_UPDATE_TIMER */
