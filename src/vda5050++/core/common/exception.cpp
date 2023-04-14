// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/common/exception.h"

#include "vda5050++/core/common/formatting.h"

vda5050pp::Error vda5050pp::core::common::errorFromInternalException(
    const std::exception &exception, const std::string &context) noexcept(true) {
  vda5050pp::Error error;

  error.errorDescription = vda5050pp::core::common::format(
      "Internal libVDA5050++ exception @({}) - \"{}\"", context, exception.what());
  error.errorType = "Internal Error";
  error.errorLevel = vda5050pp::ErrorLevel::FATAL;

  return error;
}
