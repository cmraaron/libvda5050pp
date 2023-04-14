// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/interface_agv/status/odometry.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

namespace vda5050pp::interface_agv::status::odometry {
void setAGVPosition(vda5050pp::interface_agv::Handle &handle,
                    const vda5050pp::AGVPosition &position) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().setAGVPosition(position);
}

void setLastNode(vda5050pp::interface_agv::Handle &handle,
                 const std::string &last_node_id) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  auto old_id = ha.getState().getLastNodeId();
  ha.getState().setLastNode(last_node_id);
  if (old_id != last_node_id) {
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
  }
}

void setDriving(Handle &handle, bool driving) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  bool old_driving = ha.getState().isDriving();
  ha.getState().setDriving(driving);
  if (old_driving != driving) {
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
  }
}

void setVelocity(vda5050pp::interface_agv::Handle &handle,
                 const vda5050pp::Velocity &velocity) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().setVelocity(velocity);
}
}