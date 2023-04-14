// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the declaration of the ActionDeclaredValidator
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ACTION_DECLARED_VALIDATOR_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ACTION_DECLARED_VALIDATOR_HPP_

#include <optional>
#include <string>

#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/validation/validator.h"
#include "vda5050++/model/Action.h"
#include "vda5050++/model/Error.h"

namespace vda5050pp {
namespace core {
namespace validation {
///
///\brief Check if an action is satisfied by any given action declaration
///
class ActionDeclaredValidator : public vda5050pp::core::validation::Validator<vda5050pp::Action> {
private:
  vda5050pp::core::interface_agv::HandleAccessor handle_accessor_;

  bool ctxt_edge_;
  bool ctxt_instant_;
  bool ctxt_node_;

public:
  ///
  ///\brief Construct a new Action Declared Validator object
  ///
  ///\param handle
  ///\param ctxt_node in node context
  ///\param ctxt_instant in intant context
  ///\param ctxt_edge in edge context
  ///
  ActionDeclaredValidator(vda5050pp::interface_agv::Handle &handle, bool ctxt_edge,
                          bool ctxt_instant, bool ctxt_node);

  ///
  ///\brief Run the check
  ///
  ///\param action the action to check
  ///
  std::list<vda5050pp::Error> operator()(const vda5050pp::Action &action) const;
};
}  // namespace validation
}  // namespace core
}  // namespace vda5050pp

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_ACTION_DECLARED_VALIDATOR_HPP_
