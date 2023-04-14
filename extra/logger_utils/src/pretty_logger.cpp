// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/extra/pretty_logger.h"

#include <sstream>

using namespace vda5050pp::extra;

std::vector<std::string_view> PrettyLogger::lines(const std::string &str) const {
  std::vector<std::string_view> ret;

  std::string_view sv(str);
  std::string_view::size_type pos = 0;

  while (true) {
    auto next_pos = sv.find('\n', pos);

    if (next_pos != std::string_view::npos) {
      ret.push_back(sv.substr(pos, next_pos - pos));
    } else {
      if (auto tail = sv.substr(pos); !tail.empty()) {
        ret.push_back(tail);
      }
      break;
    }

    pos = next_pos + 1;
  }

  return ret;
}

void PrettyLogger::setLogLevel(vda5050pp::interface_agv::LogLevel log_level) {
  this->vda5050pp::interface_agv::Logger::setLogLevel(log_level);
  if (this->out_logger_ != nullptr) {
    this->out_logger_->setLogLevel(log_level);
  }
}

PrettyLogger::PrettyLogger(std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger,
                           std::string_view line_prefix)
    : line_prefix_(line_prefix), out_logger_(std::move(out_logger)) {}

void PrettyLogger::setOutLogger(std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger) {
  this->out_logger_ = std::move(out_logger);
}

void PrettyLogger::logInfo(const std::string &log_message) {
  constexpr auto local_line_prefix = "[INFO]  ";
  if (this->out_logger_ == nullptr) {
    return;
  }

  std::stringstream ss;
  for (const auto &line : this->lines(log_message)) {
    ss << this->line_prefix_ << local_line_prefix << line << "\n";
  }
  this->out_logger_->logInfo(ss.str());
}
void PrettyLogger::logDebug(const std::string &log_message) {
  constexpr auto local_line_prefix = "[DEBUG] ";
  if (this->out_logger_ == nullptr) {
    return;
  }

  std::stringstream ss;
  for (const auto &line : this->lines(log_message)) {
    ss << this->line_prefix_ << local_line_prefix << line << "\n";
  }
  this->out_logger_->logDebug(ss.str());
}
void PrettyLogger::logWarn(const std::string &log_message) {
  constexpr auto local_line_prefix = "[WARN]  ";
  if (this->out_logger_ == nullptr) {
    return;
  }

  std::stringstream ss;
  for (const auto &line : this->lines(log_message)) {
    ss << this->line_prefix_ << local_line_prefix << line << "\n";
  }
  this->out_logger_->logWarn(ss.str());
}
void PrettyLogger::logError(const std::string &log_message) {
  constexpr auto local_line_prefix = "[ERROR] ";
  if (this->out_logger_ == nullptr) {
    return;
  }

  std::stringstream ss;
  for (const auto &line : this->lines(log_message)) {
    ss << this->line_prefix_ << local_line_prefix << line << "\n";
  }
  this->out_logger_->logError(ss.str());
}
void PrettyLogger::logFatal(const std::string &log_message) {
  constexpr auto local_line_prefix = "[FATAL] ";
  if (this->out_logger_ == nullptr) {
    return;
  }

  std::stringstream ss;
  for (const auto &line : this->lines(log_message)) {
    ss << this->line_prefix_ << local_line_prefix << line << "\n";
  }
  this->out_logger_->logError(ss.str());
}