// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the declaration of the HeaderTargetValidator
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_TARGET_VALIDATOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_TARGET_VALIDATOR_HPP_

#include <optional>
#include <string>

#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/validation/validator.h"
#include "vda5050++/model/Header.h"

namespace vda5050pp {
namespace core {
namespace validation {
///
///\brief Check if a header is targeting this device (manufacturer and sn)
///
class HeaderTargetValidator : public vda5050pp::core::validation::Validator<vda5050pp::Header> {
private:
  vda5050pp::core::interface_agv::HandleAccessor handle_accessor_;

public:
  explicit HeaderTargetValidator(vda5050pp::interface_agv::Handle &handle);

  ///
  ///\brief Run the check
  ///
  ///\param header the header to check
  ///
  std::list<vda5050pp::Error> operator()(const vda5050pp::Header &header) const;
};
}  // namespace validation
}  // namespace core
}  // namespace vda5050pp

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_TARGET_VALIDATOR_HPP_
