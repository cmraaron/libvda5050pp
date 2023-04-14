// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 ErrorLevel
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ERRORLEVEL_HPP_
#define INCLUDE_VDA5050PP_MODEL_ERRORLEVEL_HPP_

#include <ostream>

namespace vda5050pp {
enum ErrorLevel {

  /// warning: AGV is ready to start (e.g. maintenance
  ///          cycle expiration warning)
  WARNING,
  /// fatal: AGV is not in running condition, user
  ///        intervention required (e.g. laser scanner
  ///        is contaminated)
  FATAL
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param error_level the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const ErrorLevel &error_level) {
  switch (error_level) {
    case ErrorLevel::WARNING:
      os << "WARNING";
      break;
    case ErrorLevel::FATAL:
      os << "FATAl";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ERRORLEVEL_HPP_
