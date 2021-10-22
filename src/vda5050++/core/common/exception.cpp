// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
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
