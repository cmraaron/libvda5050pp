// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 eStop
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050_2B_2B_MODEL_ESTOP_H_
#define INCLUDE_VDA5050_2B_2B_MODEL_ESTOP_H_

#include <ostream>

namespace vda5050pp {
/// Acknowledge-Type of eStop
enum class EStop {
  /// autoAck: autoacknowledgeable e-stop is activated e.g. by bumper or
  ///          protective field
  AUTOACK,

  /// manual: e-stop has to be acknowledged manually at the vehicle
  MANUAL,

  /// remote: facility e-stop has to be acknowledged remotely
  REMOTE,

  /// none: no e-stop activated
  NONE
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param e_stop the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const EStop &e_stop) {
  switch (e_stop) {
    case EStop::AUTOACK:
      os << "AUTOACK";
      break;
    case EStop::MANUAL:
      os << "MANUAL";
      break;
    case EStop::REMOTE:
      os << "REMOTE";
      break;
    case EStop::NONE:
      os << "NONE";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050_2B_2B_MODEL_ESTOP_H_
