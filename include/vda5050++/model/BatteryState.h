// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 BatteryState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050_2B_2B_MODEL_BATTERYSTATE
#define INCLUDE_VDA5050_2B_2B_MODEL_BATTERYSTATE

#include <cstdint>
#include <optional>

namespace vda5050pp {
/// VD(M)A 5050 BatteryState
struct BatteryState {
  /// [%] State of Charge: if AGV only provides values for good or bad
  /// battery levels, these will be indicated as 20% (bad) and 80% (good).
  double batteryCharge;

  /// [V] Battery Voltage
  std::optional<double> batteryVoltage;

  /// [%] Range: [0 ... 100]
  /// State of Health
  std::optional<int8_t> batteryHealth;

  /// True: charging in progress
  /// False: AGV is currently not charging
  bool charging;

  /// [m] Range: [0.0 ... ∞)
  /// Estimated reach with current Stage of Charge
  std::optional<uint32_t> reach;

  constexpr bool operator==(const BatteryState &other) const {
    if (batteryCharge != other.batteryCharge) return false;
    if (batteryVoltage != other.batteryVoltage) return false;
    if (batteryHealth != other.batteryHealth) return false;
    if (charging != other.charging) return false;
    if (reach != other.reach) return false;
    return true;
  }
  constexpr bool operator!=(const BatteryState &other) const { return !this->operator==(other); }
};
}  // namespace vda5050pp
#endif /* INCLUDE_VDA5050_2B_2B_MODEL_BATTERYSTATE */
