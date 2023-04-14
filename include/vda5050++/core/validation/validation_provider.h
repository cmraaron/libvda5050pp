// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the ValidationProvider declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_PROVIDER
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_PROVIDER

#include <list>
#include <memory>
#include <vector>

#include "vda5050++/core/validation/validator.h"
#include "vda5050++/model/Action.h"
#include "vda5050++/model/Header.h"
#include "vda5050++/model/InstantActions.h"
#include "vda5050++/model/Order.h"

// Forward declaration, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::validation {

///
/// \brief This is the main class of the validation module, it provides access to all validation
/// facilities
///

class ValidationProvider {
private:
  std::vector<std::unique_ptr<Validator<vda5050pp::Action>>> action_validators_;
  std::unique_ptr<Validator<vda5050pp::Action>> action_edge_validator_;
  std::unique_ptr<Validator<vda5050pp::Action>> action_node_validator_;
  std::unique_ptr<Validator<vda5050pp::Action>> action_instant_validator_;
  std::vector<std::unique_ptr<Validator<vda5050pp::Header>>> header_validators_;
  std::vector<std::unique_ptr<Validator<vda5050pp::Order>>> order_validators_;

  vda5050pp::interface_agv::Handle &handle_;

  template <typename MessageObject>
  std::list<vda5050pp::Error> doValidation(const Validator<MessageObject> &validator,
                                           const MessageObject &obj) const noexcept(true);

public:
  explicit ValidationProvider(vda5050pp::interface_agv::Handle &handle);

  ///
  ///\brief Validate an Action
  ///
  ///\param ctxt_instant in instant action context
  ///\param ctxt_node in node context
  ///\param ctxt_edge in edge context
  ///
  ///\return empty if no error occurred, otherwise a list of vda5050pp::Error is returned
  ///
  std::list<vda5050pp::Error> validateAction(const vda5050pp::Action &action, bool ctxt_instant,
                                             bool ctxt_edge, bool ctxt_node) const noexcept(true);
  ///
  ///\brief Validate a Header
  ///
  ///\return empty if no error occurred, otherwise a list of vda5050pp::Error is returned
  ///
  std::list<vda5050pp::Error> validateHeader(const vda5050pp::Header &header) const noexcept(true);

  ///
  ///\brief Validate an InstantActions Message
  ///
  ///\return empty if no error occurred, otherwise a list of vda5050pp::Error is returned
  ///
  std::list<vda5050pp::Error> validateInstantActions(
      const vda5050pp::InstantActions &instant_actions) const noexcept(true);
  ///
  ///\brief Validate an Order
  ///
  ///\return empty if no error occurred, otherwise a list of vda5050pp::Error is returned
  ///
  std::list<vda5050pp::Error> validateOrder(const vda5050pp::Order &order) const noexcept(true);

  ///
  /// \brief Can the Order be ignored (duplicate orderId and orderUpdateId)
  ///
  /// \param order the order to check
  /// \return can be ignored
  ///
  bool ignoreOrder(const vda5050pp::Order &order) const noexcept(true);
};
}  // namespace vda5050pp::core::validation

#endif /* INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_PROVIDER */
