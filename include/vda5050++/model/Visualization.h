// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 Visualization
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_VISUALIZATION_HPP_
#define INCLUDE_VDA5050PP_MODEL_VISUALIZATION_HPP_

#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/Header.h"
#include "vda5050++/model/Velocity.h"

namespace vda5050pp {
/// VD(M)A 5050 Visualization message
struct Visualization {
  /// Message header
  Header header;
  /// The AGV's position
  AGVPosition agvPosition;
  /// The AGV's velocity in vehicle coordinates
  Velocity velocity;

  constexpr bool operator==(const Visualization &other) const {
    if (header != other.header) return false;
    if (agvPosition != other.agvPosition) return false;
    if (velocity != other.velocity) return false;
    return true;
  }
  constexpr bool operator!=(const Visualization &other) const { return !(this->operator==(other)); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_VISUALIZATION_HPP_
