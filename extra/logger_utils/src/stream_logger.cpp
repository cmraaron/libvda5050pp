// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/extra/stream_logger.h"

using namespace vda5050pp::extra;

StreamLogger::StreamLogger(std::ostream *debug_stream, std::ostream *info_stream,
                           std::ostream *warn_stream, std::ostream *error_stream,
                           std::ostream *fatal_stream)
    : debug_stream_(debug_stream),
      info_stream_(info_stream),
      warn_stream_(warn_stream),
      error_stream_(error_stream),
      fatal_stream_(fatal_stream) {
  this->io_mutex_by_stream_.emplace(debug_stream, std::make_unique<std::mutex>());
  this->io_mutex_by_stream_.emplace(info_stream, std::make_unique<std::mutex>());
  this->io_mutex_by_stream_.emplace(warn_stream, std::make_unique<std::mutex>());
  this->io_mutex_by_stream_.emplace(error_stream, std::make_unique<std::mutex>());
  this->io_mutex_by_stream_.emplace(fatal_stream, std::make_unique<std::mutex>());
}

void StreamLogger::logInfo(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_info) {
    std::unique_lock lock(*this->io_mutex_by_stream_[this->info_stream_]);
    *this->info_stream_ << log_message;
  }
}
void StreamLogger::logDebug(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::unique_lock lock(*this->io_mutex_by_stream_[this->debug_stream_]);
    *this->debug_stream_ << log_message;
  }
}
void StreamLogger::logWarn(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_warn) {
    std::unique_lock lock(*this->io_mutex_by_stream_[this->warn_stream_]);
    *this->warn_stream_ << log_message;
  }
}
void StreamLogger::logError(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_error) {
    std::unique_lock lock(*this->io_mutex_by_stream_[this->error_stream_]);
    *this->error_stream_ << log_message;
  }
}
void StreamLogger::logFatal(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_fatal) {
    std::unique_lock lock(*this->io_mutex_by_stream_[this->fatal_stream_]);
    *this->fatal_stream_ << log_message;
  }
}