// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 OperatingMode
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_OPERATINGMODE_HPP_
#define INCLUDE_VDA5050PP_MODEL_OPERATINGMODE_HPP_

#include <cstdint>
#include <ostream>

namespace vda5050pp {
enum class OperatingMode : uint8_t {
  /// AGV is under full control of the supervisor.
  /// AGV drives and executes actions based on
  /// orders from the supervisor
  AUTOMATIC,

  /// AGV is under control of the supervisor. AGV
  /// drives and executes actions based on orders
  /// from the supervisor. The driving speeds is
  /// controlled by the HMI. (speed can’t exceed
  /// the speed of automatic mode) The steering is
  /// under automatic control.
  /// (non-safe HMI possible)
  SEMI_AUTOMATIC,

  /// MC is not in control of the AGV. Supervisor
  /// doesn’t send driving order or actions to the
  /// AGV. HMI can be used to control the steering
  /// and velocity and handling device of the AGV.
  /// Location of the AGV is send to the MC. When
  /// AGV enters or leaves this mode, it
  /// immediately clears all the orders.
  /// (safe HMI required)
  MANUAL,

  /// MC is not in control of the AGV. MC doesn’t
  /// send driving order or actions to the AGV.
  /// Authorized personal can reconfigure the AGV.
  SERVICE,

  /// MC is not in control of the AGV. Supervisor
  /// doesn’t send driving order or actions to the
  /// AGV. The AGV is being taught, e.g. mapping is
  /// done by a MC
  TEACHIN
};

///
///\brief Write the enums value-name to an ostream
///
///\param os the stream
///\param operating_mode the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, OperatingMode &operating_mode) {
  switch (operating_mode) {
    case OperatingMode::AUTOMATIC:
      os << "AUTOMATIC";
      break;
    case OperatingMode::SEMI_AUTOMATIC:
      os << "SEMI_AUTOMATIC";
      break;
    case OperatingMode::MANUAL:
      os << "MANUAL";
      break;
    case OperatingMode::SERVICE:
      os << "SERVICE";
      break;
    case OperatingMode::TEACHIN:
      os << "TEACHIN";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}

}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_OPERATINGMODE_HPP_
