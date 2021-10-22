// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Velocity
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_VELOCITY_HPP_
#define INCLUDE_VDA5050PP_MODEL_VELOCITY_HPP_

#include <optional>

namespace vda5050pp {
/// Velocity
/// each member is optional
struct Velocity {
  /// The AGVs velocity in its x direction
  std::optional<double> vx;

  /// The AGVs velocity in its y direction
  std::optional<double> vy;

  /// The AGVs turning speed around its z axis
  std::optional<double> omega;

  constexpr bool operator==(const Velocity &other) const {
    if (vx != other.vx) return false;
    if (vy != other.vy) return false;
    if (omega != other.omega) return false;
    return true;
  }
  constexpr bool operator!=(const Velocity &other) const { return !(this->operator==(other)); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_VELOCITY_HPP_
