// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/handle.h"

#include <functional>

using namespace vda5050pp::interface_agv;

Handle::~Handle() {
  if (!this->shutdown_) {
    this->shutdown();
  }
}

void Handle::setStateUpdatePeriod(const std::chrono::system_clock::duration &period) {
  this->state_update_period_ = period;
}

void Handle::setLogger(std::shared_ptr<vda5050pp::interface_agv::Logger> logger) const {
  Logger::setCurrentLogger(logger);
}

void Handle::spin() noexcept(true) {
  using namespace std::chrono_literals;
  while (!this->shutdown_) {
    if (this->connector_passive_ != nullptr) {
      this->connector_passive_->spinOnce();
    }

    auto maybe_fn = this->task_queue_.try_pop_for(10ms);
    if (maybe_fn.has_value()) {
      maybe_fn->operator()();
    }
  }
}

void Handle::spinParallel(uint16_t num_threads) noexcept(true) {
  auto old_size = this->spinners_.size();

  if (old_size < num_threads) {
    this->spinners_.reserve(num_threads);
    while (num_threads-- > old_size) {
      this->spinners_.push_back(std::make_unique<Handle::Spinner>(*this));
    }
  } else if (old_size > num_threads) {
    auto diff = old_size - num_threads;
    auto e = this->spinners_.end();
    auto b = e - diff;
    this->spinners_.erase(b, e);
    this->spinners_.shrink_to_fit();
  }
}

Handle::SpinStatus Handle::spinOnce() noexcept(true) {
  if (!this->shutdown_) {
    if (this->connector_passive_ != nullptr) {
      this->connector_passive_->spinOnce();
    }

    auto maybe_fn = this->task_queue_.try_pop();
    if (maybe_fn.has_value()) {
      maybe_fn->operator()();
    }

    return Handle::SpinStatus::k_ok;
  } else {
    return Handle::SpinStatus::k_shutdown;
  }
}

Handle::SpinStatus Handle::spinAll() noexcept(true) {
  bool had_task = true;

  while (!this->shutdown_ && had_task) {
    if (this->connector_passive_ != nullptr) {
      this->connector_passive_->spinOnce();
    }

    auto maybe_fn = this->task_queue_.try_pop();
    if (maybe_fn.has_value()) {
      maybe_fn->operator()();
      had_task = true;
    } else {
      had_task = false;
    }
  }

  return (this->shutdown_) ? Handle::SpinStatus::k_shutdown : Handle::SpinStatus::k_ok;
}

void Handle::shutdown() noexcept(true) {
  auto logger = Logger::getCurrentLogger();
  if (logger != nullptr) {
    logger->logInfo("Shutting down the library. Order will be aborted and the MC will be informed"
                    " about the disconnect\n");
  }

  vda5050pp::Info sd_info;
  sd_info.infoDescription = "libVDA5050++ was requested to shut down.";
  sd_info.infoLevel = vda5050pp::InfoLevel::INFO;
  sd_info.infoType = "shutdown";
  this->state_manager_.addInfo(sd_info);
  this->messages_.requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_immediate);

  if (this->state_manager_.isIdle()) {
    this->shutdown_ = true;
  } else {
    this->logic_.abortOrder([this] { this->shutdown_ = true; });
  }
}

void Handle::setOdometryHandler(
    std::shared_ptr<vda5050pp::interface_agv::OdometryHandler> handler) noexcept(true) {
  this->odometry_handler_ = handler;
  if (this->odometry_handler_ != nullptr) {
    this->odometry_handler_->setHandleRef(*this);
  }
}

// Spinner /////////////////////////////////////////////////////////////////////

void Handle::Spinner::spin() {
  using namespace std::chrono_literals;
  while (!this->stop_ && !this->handle_.shutdown_) {
    if (this->handle_.connector_passive_ != nullptr) {
      this->handle_.connector_passive_->spinOnce();
    }

    auto maybe_fn = this->handle_.task_queue_.try_pop_for(10ms);
    if (maybe_fn.has_value()) {
      maybe_fn->operator()();
    }
  }
}

Handle::Spinner::Spinner(Handle &handle)
    : handle_(handle), thread_(std::bind(&Handle::Spinner::spin, this)) {}

Handle::Spinner::~Spinner() {
  this->stop_ = true;
  this->thread_.join();
}