// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#ifndef EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_STREAM_LOGGER
#define EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_STREAM_LOGGER

#include <map>
#include <memory>
#include <mutex>
#include <ostream>

#include "vda5050++/interface_agv/logger.h"

namespace vda5050pp::extra {

///
/// \brief The SteamLogger is a logger sink which assigns each level an output stream
///
class StreamLogger : public vda5050pp::interface_agv::Logger {
private:
  std::map<std::ostream *, std::unique_ptr<std::mutex>> io_mutex_by_stream_;
  std::ostream *debug_stream_;
  std::ostream *info_stream_;
  std::ostream *warn_stream_;
  std::ostream *error_stream_;
  std::ostream *fatal_stream_;

public:
  ///
  ///\brief Construct a new Stream Logger object
  ///
  ///\param debug_stream  the stream to use for logDebug()
  ///\param info_stream  the stream to use for logInfo()
  ///\param warn_stream  the stream to use for logWarn()
  ///\param error_stream the stream to use for logError()
  ///\param fatal_stream  the stream to use for logFatal()
  ///
  explicit StreamLogger(std::ostream *debug_stream, std::ostream *info_stream,
                        std::ostream *warn_stream, std::ostream *error_stream,
                        std::ostream *fatal_stream);

  ///
  ///\brief write log_message to info_stream
  ///
  ///\param log_message the log message
  ///
  void logInfo(const std::string &log_message) override;

  ///
  ///\brief write log_message to debug_stream
  ///
  ///\param log_message the log message
  ///
  void logDebug(const std::string &log_message) override;

  ///
  ///\brief write log_message to warn_stream
  ///
  ///\param log_message the log message
  ///
  void logWarn(const std::string &log_message) override;

  ///
  ///\brief write log_message to error_stream
  ///
  ///\param log_message the log message
  ///
  void logError(const std::string &log_message) override;

  ///
  ///\brief write log_message to fatal_stream
  ///
  ///\param log_message the log message
  ///
  void logFatal(const std::string &log_message) override;
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_LOGGER_UTILS_INCLUDE_VDA5050_2B_2B_EXTRA_STREAM_LOGGER */