// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file containts a virtual functor class used for message validation
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATOR_HPP_

#include <list>
#include <string>

#include "vda5050++/model/Error.h"

namespace vda5050pp {
namespace core {
namespace validation {
///
///\brief Functor for validating message objects
///
///\tparam MessageObject
///
template <typename MessageObject> class Validator {
private:
  const std::string &description_;

public:
  explicit Validator(const std::string &description) : description_(description) {}

  virtual ~Validator() = default;

  ///
  ///\brief Validate a message object with some criteria
  ///
  ///\param obj the message object to validate
  ///\return emtpy list if obj was valid \n
  ///        list of Errors if obj was invalid
  ///
  virtual std::list<vda5050pp::Error> operator()(const MessageObject &obj) const = 0;

  ///
  ///\brief Return a brief summary of the validation criteria
  ///
  ///\return std::string the summary
  ///
  const std::string &what() const noexcept(true) { return this->description_; }
};
}  // namespace validation
}  // namespace core
}  // namespace vda5050pp

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATOR_HPP_
