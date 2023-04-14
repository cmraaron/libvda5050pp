// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
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
