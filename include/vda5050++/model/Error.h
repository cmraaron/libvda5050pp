// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 Error
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ERROR_HPP_
#define INCLUDE_VDA5050PP_MODEL_ERROR_HPP_

#include <optional>
#include <string>
#include <vector>

#include "vda5050++/model/ErrorLevel.h"
#include "vda5050++/model/ErrorReference.h"

namespace vda5050pp {
/// VD(M)A 5050 Error
struct Error {
  /// Type/name of error
  std::string errorType;

  /// Array of references to identify the source of the
  /// error (e. g. headerId, orderId, actionId, ...).
  /// For additional information see
  /// best practice chapter 7
  std::optional<std::vector<ErrorReference>> errorReferences;

  /// Error description
  std::optional<std::string> errorDescription;

  /// Enum {warning, fatal}
  /// warning: AGV is ready to start (e.g. maintenance
  ///          cycle expiration warning)
  /// fatal: AGV is not in running condition, user
  ///        intervention required (e.g. laser scanner
  ///        is contaminated)
  ErrorLevel errorLevel;

  bool operator==(const Error &other) const {
    if (errorType != other.errorType) return false;
    if (errorReferences != other.errorReferences) return false;
    if (errorDescription != other.errorDescription) return false;
    if (errorLevel != other.errorLevel) return false;
    return true;
  }
  inline bool operator!=(const Error &other) const { return !this->operator==(other); }
};

}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ERROR_HPP_
