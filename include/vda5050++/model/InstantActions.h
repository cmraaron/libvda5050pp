// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 InstantActions
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_INSTANTACTIONS_HPP_
#define INCLUDE_VDA5050PP_MODEL_INSTANTACTIONS_HPP_

#include <vector>

#include "vda5050++/model/Action.h"
#include "vda5050++/model/Header.h"

namespace vda5050pp {
/// A message containing InstantActions
struct InstantActions {
  /// Message header
  Header header;

  /// List of actions to execute
  std::vector<Action> actions;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_INSTANTACTIONS_HPP_
