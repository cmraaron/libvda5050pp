// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 InfoLevel
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_INFOLEVEL_HPP_
#define INCLUDE_VDA5050PP_MODEL_INFOLEVEL_HPP_

#include <ostream>

namespace vda5050pp {
enum InfoLevel {
  /// used for debugging
  DEBUG,

  /// used for visualization
  INFO
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param info_level the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const InfoLevel &info_level) {
  switch (info_level) {
    case InfoLevel::DEBUG:
      os << "DEBUG";
      break;
    case InfoLevel::INFO:
      os << "INFO";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_INFOLEVEL_HPP_
