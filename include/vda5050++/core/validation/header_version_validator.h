// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the HeaderVersionValidator declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_VERSION_VALIDATOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_VERSION_VALIDATOR_HPP_

#include <optional>
#include <string>

#include "vda5050++/core/validation/validator.h"
#include "vda5050++/model/Error.h"
#include "vda5050++/model/Header.h"

namespace vda5050pp {
namespace core {
namespace validation {

///
/// \brief This functor checks, if the header version is compatible
///
class HeaderVersionValidator : public Validator<vda5050pp::Header> {
public:
  HeaderVersionValidator();

  ///
  ///\brief Run the check
  ///
  ///\param header the header to check
  ///\return nullopt if msg was valid \n
  ///        Error if msg was invalid
  ///
  std::list<vda5050pp::Error> operator()(const vda5050pp::Header &header) const;
};

}  // namespace validation
}  // namespace core
}  // namespace vda5050pp

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_HEADER_VERSION_VALIDATOR_HPP_
