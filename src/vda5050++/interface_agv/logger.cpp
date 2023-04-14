// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/interface_agv/logger.h"

using namespace vda5050pp::interface_agv;

std::shared_ptr<Logger> Logger::current_logger_;

std::shared_ptr<Logger> Logger::getCurrentLogger() {
  // fallback null logger
  class NullLogger : public vda5050pp::interface_agv::Logger {
  public:
    void logDebug(const std::string &) override { /* do not log */
    }
    void logInfo(const std::string &) override { /* do not log */
    }
    void logWarn(const std::string &) override { /* do not log */
    }
    void logError(const std::string &) override { /* do not log */
    }
    void logFatal(const std::string &) override { /* do not log */
    }
  };

  if (current_logger_ == nullptr) {
    current_logger_ = std::make_shared<NullLogger>();
  }

  return current_logger_;
}

void Logger::setCurrentLogger(std::shared_ptr<Logger> instance) { current_logger_ = instance; }