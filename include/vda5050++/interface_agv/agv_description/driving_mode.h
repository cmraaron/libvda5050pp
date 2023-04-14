// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the DrivingMode enumeration
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_DRIVING_MODE_HPP_
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_DRIVING_MODE_HPP_

namespace vda5050pp::interface_agv::agv_description {

enum class DrivingMode {
  ///\brief unspecified driving mode
  UNSPECIFIED = 0,
  ///\brief This AGV is limited to a physical or virtual "line"
  LINE_GUIDED = 1,
  ///\brief This AGV can navigate on its own. I.e. plan its own trajectory
  FREE = 2,
};

}

#endif  // INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_DRIVING_MODE_HPP_
