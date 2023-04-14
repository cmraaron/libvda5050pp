// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains all necessary visualization function declarations
//

#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/Visualization.h"

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_VISUALIZATION
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_VISUALIZATION

namespace vda5050pp::interface_agv::status {
///
///\brief Send visualization data
///
///\param handle the current library handle
///\param visualization the visualization data to send \n
///                     can be discarded afterwards \n
///                     NOTE: The header will be set by the library
///
void sendVisualization(Handle &handle,
                       const vda5050pp::Visualization &visualization) noexcept(false);

///
/// \brief Send a visualization message (use data in state)
///
/// \param handle the library handle
///
void sendVisualization(Handle &handle) noexcept(false);

}  // namespace vda5050pp::interface_agv::status

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STATUS_VISUALIZATION */
