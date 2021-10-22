// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the OrderIdValidator declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ORDER_ID_VALIDATOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ORDER_ID_VALIDATOR_HPP_

#include "vda5050++/core/validation/validator.h"
#include "vda5050++/interface_agv/handle.h"
#include "vda5050++/model/Order.h"

namespace vda5050pp::core::validation {

///
/// \brief This validator checks, if the OrderId and OrderUpdateId is valid
///
class OrderIdValidator : public Validator<vda5050pp::Order> {
private:
  vda5050pp::interface_agv::Handle &handle_;

public:
  explicit OrderIdValidator(vda5050pp::interface_agv::Handle &handle) noexcept(true);

  ///
  ///\brief Run the check
  ///
  ///\param order the order to check
  ///\return nullopt if msg was valid \n
  ///        Error if msg was invalid
  ///
  std::list<vda5050pp::Error> operator()(const vda5050pp::Order &order) const;
};

}  // namespace vda5050pp::core::validation

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ORDER_ID_VALIDATOR_HPP_
