// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/battery.h"

#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::setBatteryState(
    Handle &handle, const vda5050pp::BatteryState &battery_state) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().setBatteryState(battery_state);
}

vda5050pp::BatteryState vda5050pp::interface_agv::status::getBatteryState(
    const Handle &handle) noexcept(true) {
  const vda5050pp::core::interface_agv::ConstHandleAccessor ha(handle);
  return ha.getState().getBatteryState();
}