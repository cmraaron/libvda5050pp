// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the Kinematic enumeration
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_KINEMATIC_HPP_
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_KINEMATIC_HPP_

namespace vda5050pp::interface_agv::agv_description {

enum class Kinematic {
  UNSPECIFIED = 0,
  HOLONOMOUS = 1,
  ACKERMANN = 2,
  DIFFERENTIAL = 3,
};

}

#endif  // INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_KINEMATIC_HPP_
