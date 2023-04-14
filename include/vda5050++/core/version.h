// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the version constants
//

#ifndef LIBVDA5050_2B_2B_INCLUDE_VDA5050_2B_2B_CORE_VERSION
#define LIBVDA5050_2B_2B_INCLUDE_VDA5050_2B_2B_CORE_VERSION

#include <array>
#include <string_view>

namespace vda5050pp::core::version {

///
///\brief The current vda5050 version number
///
inline constexpr std::string_view current{"1.1.0"};

///
///\brief All compatible vda5050 version numbers
///
inline constexpr std::array compatible{current};

}  // namespace vda5050pp::core::version

#endif /* LIBVDA5050_2B_2B_INCLUDE_VDA5050_2B_2B_CORE_VERSION */
