// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains an implementation of a BlockingQueue
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_BLOCKING_QUEUE
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_BLOCKING_QUEUE

#include <chrono>
#include <deque>
#include <mutex>
#include <optional>
#include <queue>

#include "vda5050++/core/common/semaphore.h"

namespace vda5050pp::core::common {

///
///\brief Queue implementation used for synchronous access
///
///\tparam ValueT type of values to queue
///\tparam Container underlying containter to use (default std::deque)
///
template <typename ValueT, typename Container = std::deque<ValueT>> class BlockingQueue {
private:
  mutable std::mutex mutex_;
  std::queue<ValueT, Container> queue_;
  Semaphore<typename std::queue<ValueT, Container>::size_type> semaphore_;

public:
  ///
  ///\brief Construct a new empty BlockingQueue
  ///
  BlockingQueue() noexcept(true) : semaphore_(0) {}

  ///
  ///\brief Copy construct BlockingQueue
  ///
  /// Copy queue contents and size counter
  ///\param other BlockingQueue to copy from
  ///
  BlockingQueue(const BlockingQueue<ValueT, Container> &other) noexcept(true) {
    auto lock = std::unique_lock(other.mutex_);
    semaphore_ = Semaphore(other.size());
    queue_ = other.queue_;
  }

  ///
  ///\brief Move construct BlockingQueue
  ///
  /// Copy queue contents and size counter
  ///\param other BlockingQueue to move from
  ///
  BlockingQueue(BlockingQueue<ValueT, Container> &&other) noexcept(true)
      : queue_(std::move(other.queue_)), semaphore_(other.semaphore_) {}

  ///
  ///\brief default destructor
  ///
  ~BlockingQueue() noexcept(true) = default;

  ///
  ///\brief Copy assign BlockingQueue
  ///
  /// Copy queue contents and size counter
  ///\param other
  ///\return BlockingQueue<ValueT, Container>&
  ///
  BlockingQueue<ValueT, Container> &operator=(
      const BlockingQueue<ValueT, Container> &other) noexcept(true) {
    *this = BlockingQueue(other);
    return *this;
  }

  ///
  ///\brief Move assign BlockingQueue
  ///
  /// Move queue contents and size counter
  ///\param other
  ///\return BlockingQueue<ValueT, Container>&
  ///
  BlockingQueue<ValueT, Container> &operator=(BlockingQueue<ValueT, Container> &&other) noexcept(
      true) {
    this->queue_ = std::move(other.queue_);
    this->semaphore_ = std::move(other.semaphore_);
    return *this;
  }

  ///
  ///\brief Append element to the queue (thread safe)
  ///
  ///\param value the element to add
  ///
  void push(const ValueT &value) noexcept(false) {
    {
      auto lock = std::unique_lock(this->mutex_);
      queue_.push(value);
    }
    semaphore_.release();
  }

  ///
  ///\brief Append element to the queue (thread safe)
  ///
  ///\param value the element to add
  ///
  void push(ValueT &&value) noexcept(false) {
    {
      auto lock = std::unique_lock(this->mutex_);
      queue_.push(std::move(value));
    }
    semaphore_.release();
  }

  ///
  ///\brief pop and return the next element (blocking, thread-safe)
  ///
  /// Blocks until an element is available
  ///\return ValueT the popped element
  ///
  ValueT pop() noexcept(true) {
    semaphore_.acquire();
    auto lock = std::unique_lock(this->mutex_);
    auto value = queue_.front();
    queue_.pop();
    return value;
  }

  ///
  ///\brief pop and return the next element if available (thread-safe)
  ///
  ///\return std::optional<ValueT> value, if avaliable, otherwise std::nullopt
  ///
  std::optional<ValueT> try_pop() noexcept(true) {
    if (semaphore_.try_acquire()) {
      auto lock = std::unique_lock(this->mutex_);
      auto value = queue_.front();
      queue_.pop();
      return {value};
    } else {
      return std::nullopt;
    }
  }

  ///
  ///\brief Try to pop a value for a specific amount of time (blocking, thread-safe)
  ///
  ///\tparam Rep integral time representation
  ///\tparam Period time period (ratio)
  ///\param rel_time time to try before returning
  ///\return value if value was available \n
  ///        std::nullopt if no value was available
  ///
  template <typename Rep, typename Period>
  std::optional<ValueT> try_pop_for(const std::chrono::duration<Rep, Period> &rel_time) noexcept(
      true) {
    if (semaphore_.try_acquire_for(rel_time)) {
      auto lock = std::unique_lock(this->mutex_);
      auto value = queue_.front();
      queue_.pop();
      return {value};
    } else {
      return std::nullopt;
    }
  }

  ///
  ///\brief Try to pop a value for until a specific point in time
  ///
  ///\tparam Clock reference clock
  ///\tparam Duration duration type
  ///\param timeout_time point in time
  ///\return value if value was available \n
  ///        std::nullopt if no value was available
  ///
  template <typename Clock, typename Duration>
  std::optional<ValueT> try_pop_until(
      const std::chrono::time_point<Clock, Duration> &timeout_time) noexcept(true) {
    if (semaphore_.try_acquire_until(timeout_time)) {
      auto lock = std::unique_lock(this->mutex_);
      auto value = queue_.front();
      queue_.pop();
      return {value};
    } else {
      return std::nullopt;
    }
  }

  ///
  ///\brief return first element of the queue
  ///
  ///\return ValueT first element
  ///
  ValueT front() const noexcept(true) { return queue_.front(); }

  ///
  ///\brief return last element of the queue
  ///
  ///\return ValueT last element
  ///
  ValueT back() const noexcept(true) { return queue_.back(); }

  ///
  ///\brief Check if the queue is empty
  ///
  ///\return true queue empty
  ///\return false queue not empty
  ///
  bool empty() const noexcept(true) { return queue_.empty(); }

  ///
  ///\brief Return number of elements in the queue
  ///
  ///\return std::queue<ValueT, Container>::size_type count
  ///
  typename std::queue<ValueT, Container>::size_type size() const noexcept(true) {
    return queue_.size();
  }
};

}  // namespace vda5050pp::core::common

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_BLOCKING_QUEUE */
