// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the interruptable timer class
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_INTERRUPTABLE_TIMER
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_INTERRUPTABLE_TIMER

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace vda5050pp::core::common {

///
/// \brief InterruptableTimer return Status
///
enum class InterruptableTimerStatus {
  ///\brief Sleep finished as expected
  k_ok,
  ///\brief Sleep was interrupted
  k_interrupted,
  ///\brief The timer is currently disabled
  k_disabled,
};

///
/// \brief InterruptableTimer is a timer on which threads can do interruptable sleeps
///
/// Each timer object manages an interruptable group of sleeps.
/// Threads that whish to sleep call sleep methods. Other threads may call
/// interruptAll to interrupt all active sleep calls on *this.
///
class InterruptableTimer final {
private:
  bool terminate_ = false;
  mutable std::condition_variable sleep_cv_;
  mutable std::mutex status_mutex_;
  mutable std::condition_variable waiting_cv_;
  mutable std::mutex waiting_mutex_;
  mutable uint32_t num_waiting_ = 0;

public:
  ///
  /// \brief Construct a new enabled Interruptable Timer
  ///
  InterruptableTimer() = default;

  ///
  /// \brief Destroy the Interruptable Timer object
  ///
  /// Disable the timer and wait for all active sleep calls on *this to return
  ///
  ~InterruptableTimer() {
    this->disable();
    this->waitForClearance();
  }

  ///
  ///\brief Sleep for the specified amount of time
  ///
  ///\tparam Rep integral time representation
  ///\tparam Period time period (ratio)
  ///\param rel_time the time to sleep for
  ///\return TimerStatus
  ///
  template <typename Rep, typename Period>
  InterruptableTimerStatus sleepFor(const std::chrono::duration<Rep, Period> &rel_time) const
      noexcept(true) {
    // Check if the timer is beeing interrupted
    {
      std::unique_lock status_lock(this->status_mutex_);
      if (this->terminate_) {
        return InterruptableTimerStatus::k_disabled;
      }
    }

    // Tell interruptAll, that we start waiting
    {
      std::unique_lock waiting_lock(this->waiting_mutex_);
      this->num_waiting_++;
    }

    // Sleep
    bool term;
    {
      std::unique_lock status_lock(this->status_mutex_);
      term = this->sleep_cv_.wait_for(status_lock, rel_time, [this] { return this->terminate_; });
    }

    // Tell interruptAll, that we stopped waiting
    {
      std::unique_lock waiting_lock(this->waiting_mutex_);
      this->num_waiting_--;
    }
    this->waiting_cv_.notify_all();

    if (term) {
      return InterruptableTimerStatus::k_interrupted;
    } else {
      return InterruptableTimerStatus::k_ok;
    }
  }

  ///
  ///\brief Sleep until a certain point in time
  ///
  ///\tparam Clock reference clock
  ///\tparam Duration duration type
  ///\param timepoint point in time
  ///\return TimerStatus
  ///
  template <typename Clock, typename Duration>
  InterruptableTimerStatus sleepUntil(
      const std::chrono::time_point<Clock, Duration> &timepoint) const noexcept(true) {
    // Check if the timer is beeing interrupted
    {
      std::unique_lock status_lock(this->status_mutex_);
      if (this->terminate_) {
        return InterruptableTimerStatus::k_disabled;
      }
    }

    // Tell interruptAll, that we start waiting
    {
      std::unique_lock waiting_lock(this->waiting_mutex_);
      this->num_waiting_++;
    }

    // Sleep
    bool term;
    {
      std::unique_lock status_lock(this->status_mutex_);
      term =
          this->sleep_cv_.wait_until(status_lock, timepoint, [this] { return this->terminate_; });
    }

    // Tell interruptAll, that we stopped waiting
    {
      std::unique_lock waiting_lock(this->waiting_mutex_);
      this->num_waiting_--;
    }
    this->waiting_cv_.notify_all();

    if (term) {
      return InterruptableTimerStatus::k_interrupted;
    } else {
      return InterruptableTimerStatus::k_ok;
    }
  }

  ///
  /// \brief Interrupt all sleep calls on *this (blocks until all blocked threads resume)
  ///
  /// This effectively calls disable, then enable.
  ///
  void interruptAll() noexcept(true) {
    this->disable();
    this->enable();
  }

  ///
  /// \brief Wait until there is no active sleep call on *this
  ///
  void waitForClearance() noexcept(true) {
    std::unique_lock waiting_lock(this->waiting_mutex_);
    this->waiting_cv_.wait(waiting_lock, [this] { return this->num_waiting_ == 0; });
  }

  ///
  /// \brief Disable the timer (interrupt active sleep calls on *this)
  ///
  /// Future calls to sleep will return k_disabled
  ///
  void disable() noexcept(true) {
    {
      std::unique_lock lock(this->status_mutex_);
      this->terminate_ = true;
    }
    this->sleep_cv_.notify_all();
  }

  ///
  /// \brief Re-enable the timer after all active sleeps call on *this return (blocking)
  ///
  void enable() noexcept(true) {
    this->waitForClearance();

    std::unique_lock lock(this->status_mutex_);
    this->terminate_ = false;
  }
};

}  // namespace vda5050pp::core::common

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_INTERRUPTABLE_TIMER */
