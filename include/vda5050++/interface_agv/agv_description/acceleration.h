// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the acceleration data structure
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_ACCELERATION_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_ACCELERATION_HPP_

#include <optional>

namespace vda5050pp::interface_agv::agv_description {

///
///\brief The AGVs acceleration
///
struct Acceleration {
  ///\brief [m/s²] The AGVs acceleration on its x direction
  std::optional<double> ax;
  ///\brief
  ///\brief [m/s²] The AGVs acceleration on its y direction
  std::optional<double> ay;
  ///\brief
  ///\brief [rad/s²] The AGVs turning acceleration around its z axis
  std::optional<double> omega;
};

}  // namespace vda5050pp::interface_agv::agv_description

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_ACCELERATION_HPP_
