// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
//  TODO: Short description
//

#ifndef COMMON_INCLUDE_LOGGER_CONSOLE_LOGGER_H_
#define COMMON_INCLUDE_LOGGER_CONSOLE_LOGGER_H_

#include <iostream>

#include "vda5050++/interface_agv/logger.h"
namespace test {

class ConsoleLogger : public vda5050pp::interface_agv::Logger {
public:
  ConsoleLogger() = default;
  ~ConsoleLogger() = default;

  void logInfo(const std::string &log_message) final;
  void logDebug(const std::string &log_message) final;
  void logWarn(const std::string &log_message) final;
  void logError(const std::string &log_message) final;
  void logFatal(const std::string &log_message) final;
};

}  // namespace test

#endif  // COMMON_INCLUDE_LOGGER_CONSOLE_LOGGER_H_
