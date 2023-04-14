// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the console logger

#ifndef EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_PRETTY_LOGGER
#define EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_PRETTY_LOGGER

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "vda5050++/interface_agv/logger.h"

namespace vda5050pp::extra {

///
///\brief A pretty logger, which annotates each line with a line-prefix, then
/// forwards it to an underlying logger.
///
class PrettyLogger : public vda5050pp::interface_agv::Logger {
private:
  std::vector<std::string_view> lines(const std::string &str) const;
  std::string_view line_prefix_;
  std::unique_ptr<vda5050pp::interface_agv::Logger> out_logger_;

public:
  ///
  ///\brief Construct a new Pretty Logger object
  ///
  ///\param out_logger the output logger to use
  ///\param line_prefix prefix each log line with line_prefix
  ///
  explicit PrettyLogger(std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger,
                        std::string_view line_prefix);

  ~PrettyLogger() override = default;

  ///
  ///\brief Overwrite the current out_logger instance
  ///
  ///\param out_logger the new out_logger
  ///
  void setOutLogger(std::unique_ptr<vda5050pp::interface_agv::Logger> &&out_logger);

  ///
  ///\brief Set the LogLevel of the out_logger
  ///
  ///\param log_level the log_level to set
  ///
  void setLogLevel(vda5050pp::interface_agv::LogLevel log_level) override;

  ///
  ///\brief Forward and annotate to out_logger
  ///
  ///\param log_message the message
  ///
  void logInfo(const std::string &log_message) override;

  ///
  ///\brief Forward and annotate to out_logger
  ///
  ///\param log_message the message
  ///
  void logDebug(const std::string &log_message) override;

  ///
  ///\brief Forward and annotate to out_logger
  ///
  ///\param log_message the message
  ///
  void logWarn(const std::string &log_message) override;

  ///
  ///\brief Forward and annotate to out_logger
  ///
  ///\param log_message the message
  ///
  void logError(const std::string &log_message) override;

  ///
  ///\brief Forward and annotate to out_logger
  ///
  ///\param log_message the message
  ///
  void logFatal(const std::string &log_message) override;
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_PRETTY_LOGGER */
