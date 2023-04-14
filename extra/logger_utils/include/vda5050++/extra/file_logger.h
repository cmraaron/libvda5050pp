// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#ifndef EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_FILE_LOGGER
#define EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_FILE_LOGGER

#include <filesystem>
#include <fstream>

#include "pretty_logger.h"
#include "stream_logger.h"
#include "vda5050++/interface_agv/logger.h"

namespace vda5050pp::extra {

///
///\brief A ready-to-use File Logger
///
class FileLogger : public PrettyLogger {
private:
  std::fstream fstream_;
  std::unique_ptr<StreamLogger> mkStreamLogger(const std::filesystem::path &path) noexcept(false);

public:
  ///
  ///\brief Construct a new File Logger object
  ///
  ///\param path path of the file to append/write to
  ///
  explicit FileLogger(const std::filesystem::path &path) noexcept(false);
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_FILE_LOGGER */
