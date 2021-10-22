// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/console_logger.h"

#include <string_view>

namespace test {

constexpr std::string_view k_red_text_color = "\x1B[31m";
constexpr std::string_view k_green_text_color = "\x1B[32m";
constexpr std::string_view k_yellow_text_color = "\x1B[33m";
constexpr std::string_view k_white_text_color = "\x1B[37m";
constexpr std::string_view k_reset_text_color = "\x1B[0m";

void ConsoleLogger::logInfo(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::cout << k_white_text_color << "[INFO] " << log_message << k_reset_text_color;
  }
}
void ConsoleLogger::logDebug(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::cout << k_green_text_color << "[DEBUG] " << log_message << k_reset_text_color;
  }
}
void ConsoleLogger::logWarn(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::cout << k_yellow_text_color << "[WARN] " << log_message << k_reset_text_color;
  }
}
void ConsoleLogger::logError(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::cerr << k_red_text_color << "[ERROR] " << log_message << k_reset_text_color;
  }
}
void ConsoleLogger::logFatal(const std::string &log_message) {
  if (this->getLogLevel() <= vda5050pp::interface_agv::LogLevel::k_debug) {
    std::cerr << k_red_text_color << "[FATAL] " << log_message << k_reset_text_color;
  }
}
}  // namespace test