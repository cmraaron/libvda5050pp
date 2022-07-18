// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/operating_mode.h"

#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::setOperatingMode(
    Handle &handle, const vda5050pp::OperatingMode &operating_mode) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  auto old_mode = ha.getState().getOperatingMode();
  ha.getState().setOperatingMode(operating_mode);
  if (operating_mode != old_mode) {
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
  }
}

vda5050pp::OperatingMode vda5050pp::interface_agv::status::getOperatingMode(
    const Handle &handle) noexcept(true) {
  const vda5050pp::core::interface_agv::ConstHandleAccessor ha(handle);
  return ha.getState().getOperatingMode();
}
