// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
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
inline constexpr std::string_view current{"2.0.0"};

///
///\brief All compatible vda5050 version numbers
///
inline constexpr std::array compatible{current};

}  // namespace vda5050pp::core::version

#endif /* LIBVDA5050_2B_2B_INCLUDE_VDA5050_2B_2B_CORE_VERSION */
