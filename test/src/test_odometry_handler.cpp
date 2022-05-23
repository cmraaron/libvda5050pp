#include "test/test_odometry_handler.h"

void test::OdometryHandler::initializePosition(const vda5050pp::AGVPosition &pos) noexcept(false) {
  this->last_init_position_ = pos;

  std::unique_lock lock(this->barrier_mutex_);
  this->barrier_.wait(lock, [this] { return !this->barrier_active_; });
}

const vda5050pp::AGVPosition &test::OdometryHandler::lastInitPosition() const {
  return *this->last_init_position_;
}

bool test::OdometryHandler::hasLastInitPosition() const {
  return this->last_init_position_.has_value();
}

void test::OdometryHandler::blockFutureInitializePositionCalls() noexcept(true) {
  std::unique_lock lock(this->barrier_mutex_);
  this->barrier_active_ = true;
}

void test::OdometryHandler::unblockInitializePositionCalls() noexcept(true) {
  {
    std::unique_lock lock(this->barrier_mutex_);
    this->barrier_active_ = false;
  }
  this->barrier_.notify_all();
}