// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains function declarations for odometry storage functions
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_ODOMETRY
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_ODOMETRY

#include <functional>

#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/Velocity.h"

// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::interface_agv::status {

namespace odometry_callback {
///
///\brief This callback will be called when the library wants the AGV to publish up to date odometry
/// data
///
using UpdateOdometry = std::function<void(void)>;
}  // namespace odometry_callback

namespace odometry {
///
///\brief Set odometry data
///
///\param handle the current library handle
///\param position current position
///
void setAGVPosition(Handle &handle, const vda5050pp::AGVPosition &position) noexcept(true);

///
/// \brief set the lastNodeId field in the state
///
/// \param handle the current library handle
/// \param last_node_id the id of the last reached node
///
void setLastNode(Handle &handle, const std::string &last_node_id) noexcept(true);

///
/// \brief Overwrite state.driving
///
/// \param handle the current library handle
/// \param driving driving?
///
void setDriving(Handle &handle, bool driving) noexcept(true);

///
///\brief Set odometry data
///
///\param handle the current library handle
///\param velocity current velocity
///
void setVelocity(Handle &handle, const vda5050pp::Velocity &velocity) noexcept(true);

}  // namespace odometry
}  // namespace vda5050pp::interface_agv::status
#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_ODOMETRY */
