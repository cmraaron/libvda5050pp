// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains all necessary function declarations for load handling
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_LOADS
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_LOADS

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/Load.h"

namespace vda5050pp::interface_agv::status {
///
///\brief add a new load (enables state.loads field)
///
///\param handle the current library handle
///\param load the load to add
///
void addLoad(Handle &handle, const vda5050pp::Load &load);

///
///\brief remove a load (enables state.loads field)
///
///\param handle the current library handle
///\param load the load to remove
///
void removeLoad(Handle &handle, const vda5050pp::Load &load);

///
/// \brief Get all Loads
///
/// \param handle the current library handle
/// \return const std::vector<vda5050pp::Load>
///
std::optional<std::vector<vda5050pp::Load>> getLoads(const Handle &handle) noexcept(true);

///
/// \brief Get load by it's id
///
/// \param handle the current library handle
/// \param load_id the load id
/// \return vda5050pp::Load
///
vda5050pp::Load getLoad(const Handle &handle, const std::string &load_id) noexcept(false);

///
///\brief Disable the state.loads field
///
///\param handle the current library handle
///
void unsetLoads(Handle &handle) noexcept(true);

}  // namespace vda5050pp::interface_agv::status

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_LOADS */
