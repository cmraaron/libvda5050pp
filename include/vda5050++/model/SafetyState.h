// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 SafetyState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_SAFETYSTATUS_HPP_
#define INCLUDE_VDA5050PP_MODEL_SAFETYSTATUS_HPP_

#include "vda5050++/model/EStop.h"

namespace vda5050pp {
/// VD(M)A 5050 SafetyState
struct SafetyState {
  /// Emegerncy-Stop status
  EStop eStop;

  /// Protective field violation
  /// True: field is violated
  /// False: field is not violated
  bool fieldViolation;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_SAFETYSTATUS_HPP_
