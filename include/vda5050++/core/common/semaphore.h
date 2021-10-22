// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains a semaphore implementation
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_SEMAPHORE
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_SEMAPHORE

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <mutex>

namespace vda5050pp::core::common {

///
///\brief A counter to manage shared resource access
///
///\tparam CounterT datatype to count resources with
///
template <typename CounterT = uint16_t> class Semaphore {
private:
  CounterT avail_;
  std::mutex mutex_;
  std::condition_variable condition_variable_;

public:
  ///
  ///\brief Construct a new Semaphore object
  ///
  ///\param available starting number of managed resources (defaults to 1)
  ///
  explicit Semaphore(const CounterT available = 1) noexcept(true) : avail_(available) {}

  ///\brief only copy avail counter
  Semaphore(const Semaphore<CounterT> &other) noexcept(true) : avail_(other.avail_) {}

  ///\brief only move avail counter
  Semaphore(Semaphore<CounterT> &&other) noexcept(true) { this->avail_ = std::move(other.avail_); }

  ///\brief default destructor
  ~Semaphore() noexcept(true) = default;

  ///\brief only copy avail counter
  Semaphore<CounterT> &operator=(const Semaphore<CounterT> &other) noexcept(true) {
    *this = Semaphore(other);
    return *this;
  }

  ///\brief only move avail counter
  Semaphore<CounterT> &operator=(Semaphore<CounterT> &&other) noexcept(true) {
    this->avail_ = std::move(other.avail_);
    return *this;
  }

  ///
  ///\brief Acquire resource (blocking)
  ///
  /// This function blocks until a resource had been acquired
  ///
  void acquire() noexcept(true) {
    {
      std::unique_lock lock(mutex_);
      condition_variable_.wait(lock, [this] { return this->avail_ > 0; });
      avail_--;
    }
    condition_variable_.notify_one();
  }

  ///
  ///\brief Acquire resource (non-blocking)
  ///
  ///\return Returns true if resource was acquired, false otherwise
  ///
  bool try_acquire() noexcept(true) {
    std::unique_lock lock(this->mutex_, std::defer_lock);
    if (lock.try_lock()) {
      if (avail_ > 0) {
        avail_--;
      } else {
        lock.unlock();
        return false;
      }
      lock.unlock();
      condition_variable_.notify_one();
      return true;
    }
    return false;
  }

  ///
  ///\brief Try to acquire a resource for a specific amount of time
  ///
  /// Blocks until either a resource was acquired or the timeout was reached
  ///
  ///\tparam Rep integral time representation
  ///\tparam Period time period (ratio)
  ///\param rel_time the maximum amount of time to try to acquire the resource
  ///\return true resource acquired \n
  ///        false resource not acquired
  ///
  template <typename Rep, typename Period>
  bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) noexcept(true) {
    std::unique_lock lock(mutex_);
    if (condition_variable_.wait_for(lock, rel_time, [this] { return this->avail_ > 0; })) {
      avail_--;
      lock.unlock();
      condition_variable_.notify_one();
      return true;
    } else {
      return false;
    }
  }

  ///
  ///\brief Try to acquire a resource for until a specific point in time
  ///
  ///\tparam Clock reference clock
  ///\tparam Duration duration type
  ///\param timeout_time point in time
  ///\return true resource was acquired \n
  ///        false resource was not acquired
  ///
  template <typename Clock, typename Duration>
  bool try_acquire_until(const std::chrono::time_point<Clock, Duration> &timeout_time) noexcept(
      true) {
    std::unique_lock lock(mutex_);
    if (condition_variable_.wait_until(lock, timeout_time, [this] { return this->avail_ > 0; })) {
      avail_--;
      lock.unlock();
      condition_variable_.notify_one();
      return true;
    } else {
      return false;
    }
  }

  ///
  ///\brief Release a resource
  ///
  void release() noexcept(true) {
    {
      std::unique_lock lock(this->mutex_);
      avail_++;
    }
    condition_variable_.notify_one();
  }

  ///
  ///\brief Maximum value of resources to manage with CounterT
  ///
  ///\return maximum value
  ///
  constexpr CounterT max() noexcept(true) { return std::numeric_limits<CounterT>::max(); }
};

}  // namespace vda5050pp::core::common

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_SEMAPHORE */
