// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the parameter range data structure
//

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_PARAMETER_RANGE_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_PARAMETER_RANGE_HPP_

#include <optional>
#include <set>
#include <string>

#include "vda5050++/interface_agv/agv_description/serialized_value.h"

namespace vda5050pp::interface_agv::agv_description {
struct ParameterRange {
  ///
  ///\brief The Parameter's Key
  ///
  /// This also identifies this ParameterRange among others
  ///
  std::string key;

  ///
  ///\brief JSON-Serialized minimum value type.
  ///
  /// Optional for ordinal value types.
  ///
  std::optional<SerializedValue> ordinal_min;

  ///
  ///\brief JSON-Serialized maximum value type.
  ///
  /// Optional for ordinal value types.
  ///
  std::optional<SerializedValue> ordinal_max;

  ///
  ///\brief A list of allowed values
  ///
  /// Optional for enumerating values
  ///
  std::optional<std::set<std::string>> value_set;

  ///
  ///\brief comparison for set ordering
  ///
  ///\param other the ParameterRange to compare to
  ///\return this < other
  ///
  bool operator<(const ParameterRange &other) const { return this->key < other.key; }
};
}  // namespace vda5050pp::interface_agv::agv_description

#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_PARAMETER_RANGE_HPP_
