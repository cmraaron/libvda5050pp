// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the console logger

#ifndef EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_MULTIPLEX_LOGGER
#define EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_MULTIPLEX_LOGGER

#include <initializer_list>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>

#include "vda5050++/interface_agv/logger.h"

namespace vda5050pp::extra {

///
///\brief A MutliplexLogger fans out all log messages to a given set of sink loggers.
///
class MultiplexLogger : public vda5050pp::interface_agv::Logger {
private:
  std::shared_mutex out_logger_mutex_;
  std::set<std::unique_ptr<vda5050pp::interface_agv::Logger>> out_logger_;

public:
  ///
  /// \brief Construct a new MultiplexLogger
  ///
  /// NOTE: std::set is used instead of std::initializer_list, because of move semantics
  ///
  /// \param out_logger all logger sinks
  ///
  explicit MultiplexLogger(
      std::set<std::unique_ptr<vda5050pp::interface_agv::Logger>> &&out_logger);

  ///
  ///\brief Set the LogLevel for all out_logger instances
  ///
  ///\param log_level the log_level to set
  ///
  void setLogLevel(vda5050pp::interface_agv::LogLevel log_level) override;

  ///
  ///\brief forward info message to all out_logger
  ///
  ///\param log_message the message
  ///
  void logInfo(const std::string &log_message) final;

  ///
  ///\brief forward debug message to all out_logger
  ///
  ///\param log_message the message
  ///
  void logDebug(const std::string &log_message) final;

  ///
  ///\brief forward warn message to all out_logger
  ///
  ///\param log_message the message
  ///
  void logWarn(const std::string &log_message) final;

  ///
  ///\brief forward error message to all out_logger
  ///
  ///\param log_message the message
  ///
  void logError(const std::string &log_message) final;

  ///
  ///\brief forward fatal message to all out_logger
  ///
  ///\param log_message the message
  ///
  void logFatal(const std::string &log_message) final;

  ///
  ///\brief add a new out_logger
  ///
  ///\param out_logger the new logger
  ///
  void addOutputLogger(std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger);
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_MULTIPLEX_LOGGER */
