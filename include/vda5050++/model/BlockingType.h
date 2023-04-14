// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 BlockingType
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_BLOCKINGTYPE_HPP_
#define INCLUDE_VDA5050PP_MODEL_BLOCKINGTYPE_HPP_

#include <ostream>

namespace vda5050pp {
/// Enums for blockingType
enum class BlockingType {
  /// “NONE” – allows driving and other actions
  NONE,
  /// “SOFT” – allows other actions, but not driving
  SOFT,
  /// “HARD” – is the only allowed action at that time.
  HARD
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param blocking_type the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const BlockingType &blocking_type) {
  switch (blocking_type) {
    case BlockingType::NONE:
      os << "NONE";
      break;
    case BlockingType::SOFT:
      os << "SOFT";
      break;
    case BlockingType::HARD:
      os << "HARD";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_BLOCKINGTYPE_HPP_
