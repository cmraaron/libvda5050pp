// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
//

#include "vda5050++/extra/multiplex_logger.h"

using namespace vda5050pp::extra;

MultiplexLogger::MultiplexLogger(
    std::set<std::unique_ptr<vda5050pp::interface_agv::Logger>> &&out_logger)
    : vda5050pp::interface_agv::Logger(), out_logger_(std::move(out_logger)) {
  // Set log level to default (info)
  for (auto &logger : this->out_logger_) {
    if (logger != nullptr) {
      logger->setLogLevel(vda5050pp::interface_agv::LogLevel::k_info);
    }
  }
}

void MultiplexLogger::setLogLevel(vda5050pp::interface_agv::LogLevel log_level) {
  this->vda5050pp::interface_agv::Logger::setLogLevel(log_level);
  std::unique_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->setLogLevel(log_level);
    }
  }
}

void MultiplexLogger::logInfo(const std::string &log_message) {
  std::shared_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->logInfo(log_message);
    }
  }
}
void MultiplexLogger::logDebug(const std::string &log_message) {
  std::shared_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->logDebug(log_message);
    }
  }
}
void MultiplexLogger::logWarn(const std::string &log_message) {
  std::shared_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->logWarn(log_message);
    }
  }
}
void MultiplexLogger::logError(const std::string &log_message) {
  std::shared_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->logError(log_message);
    }
  }
}
void MultiplexLogger::logFatal(const std::string &log_message) {
  std::shared_lock lock(this->out_logger_mutex_);
  for (auto &out_logger : this->out_logger_) {
    if (out_logger != nullptr) {
      out_logger->logFatal(log_message);
    }
  }
}

void MultiplexLogger::addOutputLogger(
    std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger) {
  std::unique_lock lock(this->out_logger_mutex_);
  out_logger->setLogLevel(this->getLogLevel());
  this->out_logger_.emplace(std::move(out_logger));
}