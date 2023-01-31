// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 OrientationType
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050_2B_2B_MODEL_ORIENTATIONTYPE_H_
#define INCLUDE_VDA5050_2B_2B_MODEL_ORIENTATIONTYPE_H_

#include <cstdint>
#include <ostream>

namespace vda5050pp {

enum class OrientationType : uint8_t {
  /// tangential to the edge.
  TANGENTIAL,

  /// relative to the global project specific map coordinate system
  GLOBAL
};

///
///\brief Write the enums value-name to an ostream
///
///\param os the stream
///\param operating_mode the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, OrientationType &orientation_type) {
  switch (orientation_type) {
    case OrientationType::TANGENTIAL:
      os << "TANGENTIAL";
      break;
    case OrientationType::GLOBAL:
      os << "GLOBAL";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}

}  // namespace vda5050pp

#endif  // INCLUDE_VDA5050_2B_2B_MODEL_ORIENTATIONTYPE_H_
