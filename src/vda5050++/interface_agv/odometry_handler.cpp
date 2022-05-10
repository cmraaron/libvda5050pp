#include "vda5050++/interface_agv/odometry_handler.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/interface_agv/status/visualization.h"

using namespace vda5050pp::interface_agv;

void OdometryHandler::setHandleRef(Handle &handle) noexcept(true) { this->handle_ptr_ = &handle; }

OdometryHandler::NotAttachedError::NotAttachedError() noexcept(true)
    : std::logic_error("OdometryHandler was not attached to the library handle") {}

OdometryHandler::InitializePositionError::InitializePositionError(const std::string &msg) noexcept(
    true)
    : std::runtime_error(msg) {}

void OdometryHandler::setAGVPosition(const vda5050pp::AGVPosition &pos) noexcept(false) {
  if (this->handle_ptr_ == nullptr) {
    throw NotAttachedError();
  }

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_ptr_);

  ha.getState().setAGVPosition(pos);
}

OdometryHandler::~OdometryHandler() { this->disableAutomaticVisualizationMessages(); }

void OdometryHandler::setVelocity(const vda5050pp::Velocity &vel) noexcept(false) {
  if (this->handle_ptr_ == nullptr) {
    throw NotAttachedError();
  }

  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_ptr_);

  ha.getState().setVelocity(vel);
}

void OdometryHandler::enableAutomaticVisualizationMessages(
    std::chrono::system_clock::duration period) noexcept(false) {
  if (this->handle_ptr_ == nullptr) {
    throw NotAttachedError();
  }

  this->disableAutomaticVisualizationMessages();

  auto vis_task = [this, period] {
    auto running = true;
    auto wakeup_time = std::chrono::system_clock::now() + period;
    auto wakeup_time_2 = wakeup_time + period;

    while (running) {
      switch (this->visualization_timer_.sleepUntil(wakeup_time)) {
        case core::common::InterruptableTimerStatus::k_ok:
          vda5050pp::interface_agv::status::sendVisualization(*this->handle_ptr_);
          wakeup_time = wakeup_time_2;
          wakeup_time_2 = wakeup_time + period;
          break;
        case core::common::InterruptableTimerStatus::k_interrupted:
          // Do nothing and sleep again next iteration
          break;
        default:
        case core::common::InterruptableTimerStatus::k_disabled:
          running = false;
          break;
      }
    }
  };

  this->visualization_timer_.enable();
  this->visualization_thread_ = std::make_unique<std::thread>(vis_task);
}
void OdometryHandler::disableAutomaticVisualizationMessages() noexcept(true) {
  this->visualization_timer_.disable();
  if (this->visualization_thread_ != nullptr) {
    this->visualization_thread_->join();
  }
}

bool OdometryHandler::isAttached() const noexcept(true) { return this->handle_ptr_ != nullptr; }