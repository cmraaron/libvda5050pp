// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the IdNotFoundError class
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ID_NOT_FOUND_ERROR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ID_NOT_FOUND_ERROR_HPP_

#include <stdexcept>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

///
/// \brief Special std::runtime_error for missing PTN ids
///
class IdNotFoundError : std::runtime_error {
public:
  explicit IdNotFoundError(const LogicTaskNetID &id)
      : std::runtime_error(vda5050pp::core::common::logstring("Invalid internal State, PTN ID (",
                                                              id, ") not found.")) {}
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ID_NOT_FOUND_ERROR_HPP_
