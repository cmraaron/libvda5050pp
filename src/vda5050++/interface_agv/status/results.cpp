// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/interface_agv/status/results.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

void vda5050pp::interface_agv::status::addError(Handle &handle,
                                                const vda5050pp::Error &error) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().addError(error);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void vda5050pp::interface_agv::status::ensureError(Handle &handle,
                                                   const vda5050pp::Error &error) noexcept(true){
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  if (ha.getState().ensureError(error)) {
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
  }
}

void vda5050pp::interface_agv::status::addInfo(Handle &handle,
                                               const vda5050pp::Info &info) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().addInfo(info);
}

void vda5050pp::interface_agv::status::removeError(Handle &handle,
                                                   const vda5050pp::Error &error) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  auto nr = ha.getState().removeError([&error](const auto &other) {
    return !(
        error.errorType != other.errorType || error.errorDescription != other.errorDescription ||
        error.errorLevel != other.errorLevel || error.errorReferences != other.errorReferences);
  });
  if (nr > 0) {
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
  }
}

void vda5050pp::interface_agv::status::removeInfo(Handle &handle,
                                                  const vda5050pp::Info &info) noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(handle);
  ha.getState().removeInfo([&info](const auto &other) {
    return !(info.infoType != other.infoType || info.infoDescription != other.infoDescription ||
             info.infoLevel != other.infoLevel || info.infoReferences != other.infoReferences);
  });
}