// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 InfoReference
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_INFOREFERENCE_HPP_
#define INCLUDE_VDA5050PP_MODEL_INFOREFERENCE_HPP_

#include <string>

namespace vda5050pp {
/// Note: Not defined in VD(M)A5050 Version 2.0, but derived from ErrorReference due to consistency
/// reasons
struct InfoReference {
  /// References the type of reference (e.g. orderId, headerId, actionId, ...)
  std::string referenceKey;

  /// References the value, which belongs to the key.
  std::string referenceValue;

  bool operator==(const InfoReference &other) const {
    if (referenceKey != other.referenceKey) return false;
    if (referenceValue != other.referenceValue) return false;
    return true;
  }
  inline bool operator!=(const InfoReference &other) const { return !this->operator==(other); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_INFOREFERENCE_HPP_
