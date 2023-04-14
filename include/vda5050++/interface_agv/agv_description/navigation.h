// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the Navigation data structure
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_NAVIGATION_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_NAVIGATION_HPP_

#include "vda5050++/interface_agv/agv_description/acceleration.h"
#include "vda5050++/interface_agv/agv_description/driving_mode.h"
#include "vda5050++/interface_agv/agv_description/kinematic.h"
#include "vda5050++/interface_agv/agv_description/velocity.h"

namespace vda5050pp::interface_agv::agv_description {

struct Navigation {
  DrivingMode driving_mode;

  Kinematic kinematic;

  Velocity max_velocity;

  Acceleration max_acceleration;

  Acceleration max_deceleration;

  /// [m] Minimum turning radius of the AGV
  double min_turning_radius;
};

}  // namespace vda5050pp::interface_agv::agv_description

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_NAVIGATION_HPP_
