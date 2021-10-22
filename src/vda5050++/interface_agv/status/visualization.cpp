// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/visualization.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::sendVisualization(
    Handle &handle, const vda5050pp::Visualization &visualization) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getMessages().sendVisualization(visualization);
}

void vda5050pp::interface_agv::status::sendVisualization(Handle &handle) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  auto maybe_pos = ha.getState().getAGVPosition();
  auto maybe_vel = ha.getState().getVelocity();
  if (maybe_pos.has_value() && maybe_vel.has_value()) {
    vda5050pp::Visualization vis;
    vis.agvPosition = *maybe_pos;
    vis.velocity = *maybe_vel;
    ha.getMessages().sendVisualization(vis);
  }
}