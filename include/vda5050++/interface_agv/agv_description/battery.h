// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the Battery data structure
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_BATTERY_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_BATTERY_HPP_

#include <cstdint>
#include <optional>
#include <string>

namespace vda5050pp::interface_agv::agv_description {

///
///\brief A battery description of an AGV
///
struct Battery {
  ///\brief [m] Maximum reach with full Battery
  std::optional<uint32_t> max_reach;
  ///\brief [Ah] Maximum Battery Charge
  double max_charge;
  ///\brief [V] Maximum Battery Voltage
  double max_voltage;
  ///
  ///\brief The charge type of the battery
  ///
  /// I.e. what kind of charge station can be used.
  ///
  std::string charge_type;
};
}  // namespace vda5050pp::interface_agv::agv_description

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_BATTERY_HPP_
