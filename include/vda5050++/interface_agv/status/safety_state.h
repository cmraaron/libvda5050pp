// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains necessary safety state function declarations
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_STATUS_SAFETY_STATE_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_STATUS_SAFETY_STATE_HPP_

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/SafetyState.h"

namespace vda5050pp::interface_agv::status {
///
///\brief Set the Safety State object
///
///\param handle the current library handle
///\param safety_state safety state
///
void setSafetyState(Handle &handle, const vda5050pp::SafetyState &safety_state);

///
///\brief get const reference of the current SafetyState
///
///\param handle the current library handle
///\return vda5050pp::SafetyState
///
vda5050pp::SafetyState getSafetyState(const Handle &handle) noexcept(true);
}  // namespace vda5050pp::interface_agv::status

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_STATUS_SAFETY_STATE_HPP_
