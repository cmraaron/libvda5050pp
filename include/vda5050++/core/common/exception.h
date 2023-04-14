// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains exception function declarations
//
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_EXCEPTION
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_EXCEPTION

#include <stdexcept>
#include <string>

#include "vda5050++/model/Error.h"

namespace vda5050pp::core::common {

vda5050pp::Error errorFromInternalException(const std::exception &exception,
                                            const std::string &context = "") noexcept(true);

}

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_EXCEPTION */
