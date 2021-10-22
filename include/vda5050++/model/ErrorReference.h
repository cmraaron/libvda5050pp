// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 ErrorReference
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ERRORREFERENCE_HPP_
#define INCLUDE_VDA5050PP_MODEL_ERRORREFERENCE_HPP_

#include <string>

namespace vda5050pp {
/// Desribes a variable and it's value related to the error
struct ErrorReference {
  /// References the type of reference (e. g. headerId, orderId, actionId, …).
  std::string referenceKey;

  /// References the value of the reference key.
  std::string referenceValue;

  bool operator==(const ErrorReference &other) const {
    if (referenceKey != other.referenceKey) return false;
    if (referenceValue != other.referenceValue) return false;
    return true;
  }
  inline bool operator!=(const ErrorReference &other) const { return !this->operator==(other); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ERRORREFERENCE_HPP_
