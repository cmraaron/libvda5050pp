// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains necessary battery function declarations
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_BATTERY
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_BATTERY

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/BatteryState.h"

namespace vda5050pp::interface_agv::status {
///
///\brief Set the Battery State object
///
///\param handle the current library handle
///\param battery_state object
///
void setBatteryState(Handle &handle, const vda5050pp::BatteryState &battery_state) noexcept(true);

///
///\brief Get const reference of current battery state
///
///\param handle the current library handle
///\return vda5050pp::BatteryState
///
vda5050pp::BatteryState getBatteryState(const Handle &handle) noexcept(true);

}  // namespace vda5050pp::interface_agv::status

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_BATTERY */
