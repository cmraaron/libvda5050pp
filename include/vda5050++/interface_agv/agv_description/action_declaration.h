// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the ActionDeclaration struct

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_ACTION_DECLARATION
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_ACTION_DECLARATION

#include <set>
#include <string>

#include "vda5050++/interface_agv/agv_description/parameter_range.h"
#include "vda5050++/model/Action.h"
#include "vda5050++/model/BlockingType.h"

namespace vda5050pp::interface_agv::agv_description {
/// \brief Declaration for an VD(M)A 5050 action the AGV can perform
///
/// Used to auto-validate incoming actions
struct ActionDeclaration {
  ///
  /// \brief action type identifier
  ///
  /// This also identifies the action declaration among others
  ///
  std::string action_type;

  /// \brief all mandatory parameter keys (unique per key)
  std::set<ParameterRange> parameter;

  /// \brief all optional parameter keys (unique per key)
  std::set<ParameterRange> optional_parameter;

  /// \brief all allowed blocing types
  std::set<BlockingType> blocking_types;

  ///\brief Can this action be instant?
  bool instant = false;

  ///\brief Can this action be attached to a node?
  bool node = false;

  ///\brief Can this action be attached to an edge?
  bool edge = false;

  ///
  ///\brief Check this is less then some other declaration
  ///
  /// Used for set ordering
  ///
  ///\param other the other declaration
  ///\return true if this is less then other
  ///\return false if other is greater or equal to this
  ///
  bool operator<(const ActionDeclaration &other) const {
    return this->action_type < other.action_type;
  }
};
}  // namespace vda5050pp::interface_agv::agv_description
#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_ACTION_DECLARATION */
