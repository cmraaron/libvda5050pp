// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
//  This file contains the Logger Interface for the library
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_LOGGER
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_LOGGER

#include <memory>
#include <string>

namespace vda5050pp::interface_agv {

///\brief log level for the logger
enum class LogLevel { k_debug, k_info, k_warn, k_error, k_fatal };

///
/// \brief Logger interface used by the library
///
class Logger {
private:
  static std::shared_ptr<Logger> current_logger_;

public:
  ///
  /// \brief Get the currently set logger
  ///
  /// \return std::shared_ptr<Logger>
  ///
  static std::shared_ptr<Logger> getCurrentLogger();

  ///
  /// \brief Overwrite the currently set logger
  ///
  /// \param instance
  ///
  static void setCurrentLogger(std::shared_ptr<Logger> instance);

  virtual ~Logger() = default;

  ///
  /// \brief Set the curent loglevel
  ///
  /// \param logLevel
  ///
  virtual void setLogLevel(LogLevel logLevel) { this->log_level_ = logLevel; };

  ///
  /// \brief Get the current loglevel
  ///
  /// \return LogLevel
  ///
  virtual LogLevel getLogLevel() { return this->log_level_; }

  ///
  /// \brief log info message
  ///
  /// \param log_message
  ///
  virtual void logInfo(const std::string &log_message) = 0;

  ///
  /// \brief log debug message
  ///
  /// \param log_message
  ///
  virtual void logDebug(const std::string &log_message) = 0;

  ///
  /// \brief log warn message
  ///
  /// \param log_message
  ///
  virtual void logWarn(const std::string &log_message) = 0;

  ///
  /// \brief log error message
  ///
  /// \param log_message
  ///
  virtual void logError(const std::string &log_message) = 0;

  ///
  /// \brief log fatal message
  ///
  /// \param log_message
  ///
  virtual void logFatal(const std::string &log_message) = 0;

private:
  LogLevel log_level_ = LogLevel::k_info;
};

}  // namespace vda5050pp::interface_agv
#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_LOGGER */
