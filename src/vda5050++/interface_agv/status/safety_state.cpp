// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/safety_state.h"

#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::setSafetyState(Handle &handle,
                                                      const vda5050pp::SafetyState &safety_state) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().setSafetyState(safety_state);
}

vda5050pp::SafetyState vda5050pp::interface_agv::status::getSafetyState(
    const Handle &handle) noexcept(true) {
  vda5050pp::core::interface_agv::ConstHandleAccessor ha(handle);
  return ha.getState().getSafetyState();
}