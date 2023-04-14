// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains all necessary function declarations for info and error storage
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_RESULTS
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_RESULTS

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/Error.h"
#include "vda5050++/model/Info.h"

namespace vda5050pp::interface_agv::status {

///
///\brief Add a new Error
///
///\param handle the current library handle
///\param error the error to add
///
void addError(Handle &handle, const vda5050pp::Error &error) noexcept(true);

///
/// \brief Ensure a certain error is set
/// 
/// \param handle the current library handle
/// \param error the error to add
///
void ensureError(Handle &handle, const vda5050pp::Error &error) noexcept(true);

///
///\brief Add a new Info
///
///\param handle the current library handle
///\param info the info to add
///
void addInfo(Handle &handle, const vda5050pp::Info &info) noexcept(true);

///
///\brief remove an existing Error
///
///\param error the error to remove
///\param handle the current library handle
///
void removeError(Handle &handle, const vda5050pp::Error &error) noexcept(true);

///
///\brief remove an existing Info
///
///\param info the Info to remove
///\param handle the current library handle
///
void removeInfo(Handle &handle, const vda5050pp::Info &info) noexcept(true);

// TODO: get

}  // namespace vda5050pp::interface_agv::status
#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_RESULTS */
