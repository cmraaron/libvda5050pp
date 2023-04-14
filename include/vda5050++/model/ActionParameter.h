// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 ActionParameter
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ACTIONPARAMETER_HPP_
#define INCLUDE_VDA5050PP_MODEL_ACTIONPARAMETER_HPP_

#include <string>

namespace vda5050pp {
/// A key-value-pair
/// The value will be deserialized to an actual datatype
struct ActionParameter {
  /// Action key
  std::string key;

  /// Value of the action key
  std::string value;

  ///
  ///\brief Check if equal to another ActionParameter
  ///
  /// Does not check value.value
  ///\param other ActionParamater to compare to
  ///\return equal?
  ///
  bool operator==(const ActionParameter &other) const noexcept(true) {
    if (key != other.key) return false;
    if (value != other.value) return false;
    return true;
  }

  ///
  ///\brief Check if not equal to another ActionParameter
  ///
  /// Does not check value.value
  ///\param other ActionParamater to compare to
  ///\return not equal?
  ///
  inline bool operator!=(const ActionParameter &other) const noexcept(true) {
    return !this->operator==(other);
  }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ACTIONPARAMETER_HPP_
