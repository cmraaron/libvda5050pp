// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains necessary operating mode function declarations
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_OPERATING_MODE
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_OPERATING_MODE

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/OperatingMode.h"

namespace vda5050pp::interface_agv::status {
///
///\brief Set the Operating Mode object
///
///\param handle the current library handle
///\param operating_mode operating mode
///
void setOperatingMode(Handle &handle,
                      const vda5050pp::OperatingMode &operating_mode) noexcept(true);

///
///\brief Get the current OperatingMode (may be unset)
///
///\param handle the current library handle
///\return vda5050pp::OperatingMode
///
vda5050pp::OperatingMode getOperatingMode(const Handle &handle) noexcept(true);
}  // namespace vda5050pp::interface_agv::status

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_OPERATING_MODE */
