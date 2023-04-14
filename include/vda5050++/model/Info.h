// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 Info
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_INFO_HPP_
#define INCLUDE_VDA5050PP_MODEL_INFO_HPP_

#include <optional>
#include <string>
#include <vector>

#include "vda5050++/model/InfoLevel.h"
#include "vda5050++/model/InfoReference.h"

namespace vda5050pp {
/// Message type for debugging and or visualization information
struct Info {
  /// Type/name of info
  std::string infoType;

  /// Array of references to identify the source of
  /// the error. (e. g. headerId,orderId,actionId,...)
  std::optional<std::vector<InfoReference>> infoReferences;

  /// Info description
  std::optional<std::string> infoDescription;

  /// Debugging or visualization
  InfoLevel infoLevel;

  bool operator==(const Info &other) const {
    if (infoType != other.infoType) return false;
    if (infoReferences != other.infoReferences) return false;
    if (infoDescription != other.infoDescription) return false;
    if (infoLevel != other.infoLevel) return false;
    return true;
  }
  inline bool operator!=(const Info &other) const { return !this->operator==(other); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_INFO_HPP_
