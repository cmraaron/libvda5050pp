// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the Velocity data structure
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_VELOCITY_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_VELOCITY_HPP_

#include <optional>

namespace vda5050pp::interface_agv::agv_description {

struct Velocity {
  /// [m/s] The AGVs maximum velocity in linear motion
  double linear;
  /// [rad/s] The AGVs maximum velocity in angular motion
  std::optional<double> angular;
};

}  // namespace vda5050pp::interface_agv::agv_description

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_VELOCITY_HPP_
