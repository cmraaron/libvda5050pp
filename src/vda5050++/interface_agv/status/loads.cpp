// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/loads.h"

#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::addLoad(Handle &handle, const vda5050pp::Load &load) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().addLoad(load);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void vda5050pp::interface_agv::status::removeLoad(Handle &handle, const vda5050pp::Load &load) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().removeLoad(load);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

std::optional<std::vector<vda5050pp::Load>> vda5050pp::interface_agv::status::getLoads(
    const Handle &handle) noexcept(true) {
  vda5050pp::core::interface_agv::ConstHandleAccessor ha(handle);
  return ha.getState().getLoads();
}

vda5050pp::Load vda5050pp::interface_agv::status::getLoad(
    const Handle &handle, const std::string &load_id) noexcept(false) {
  vda5050pp::core::interface_agv::ConstHandleAccessor ha(handle);
  return ha.getState().getLoad(load_id);
}

void vda5050pp::interface_agv::status::unsetLoads(Handle &handle) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  return ha.getState().unsetLoads();
}