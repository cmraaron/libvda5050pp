// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the console logger

#ifndef EXTRA_CONSOLE_LOGGER_VDA5050_2B_2B_EXTRA_CONSOLE_LOGGER
#define EXTRA_CONSOLE_LOGGER_VDA5050_2B_2B_EXTRA_CONSOLE_LOGGER

#include <string>
#include <string_view>

#include "pretty_logger.h"
#include "vda5050++/interface_agv/logger.h"

namespace vda5050pp::extra {

///
///\brief A ready-to-use Console Logger
///
class ConsoleLogger : public vda5050pp::extra::PrettyLogger {
public:
  ///
  ///\brief Construct a new ConsoleLogger object
  ///
  ///\param line_prefix prefix each log line with line_prefix
  ///
  explicit ConsoleLogger(std::string_view line_prefix = "[libVDA5050++]");

  ~ConsoleLogger() override = default;
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_CONSOLE_LOGGER_VDA5050_2B_2B_EXTRA_CONSOLE_LOGGER */
