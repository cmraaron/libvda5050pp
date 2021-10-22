// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 ActionState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ACTIONSTATE_HPP_
#define INCLUDE_VDA5050PP_MODEL_ACTIONSTATE_HPP_

#include <optional>
#include <string>

#include "vda5050++/model/ActionStatus.h"

namespace vda5050pp {
/// VD(M)A 5050 ActionState
struct ActionState {
  /// action_ID
  std::string actionId;

  /// actionType of the action.
  /// Optional: Only for informational purposes.
  /// Order knows the type.
  /// Note: Not explcitly flagged optional after VD(M)A 5050,
  /// but more consisten after the description
  std::optional<std::string> actionType;

  /// Additional information on the current action
  std::optional<std::string> actionDescription;

  /// The current progress of the action
  ActionStatus actionStatus;

  /// Description of the result, e.g. the result of a RFID-read.
  /// Errors will be transmitted in errors. Examples for results are
  /// given in 6.5
  std::optional<std::string> resultDescription;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ACTIONSTATE_HPP_
