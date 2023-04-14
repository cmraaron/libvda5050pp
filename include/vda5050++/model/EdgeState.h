// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 EdgeState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_EDGESTATE_HPP_
#define INCLUDE_VDA5050PP_MODEL_EDGESTATE_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "vda5050++/model/Trajectory.h"

namespace vda5050pp {
/// VD(M)A 5050 EdgeState
struct EdgeState {
  /// Unique edge identification
  std::string edgeId;

  /// sequenceId to differentiate between multiple edges with
  /// the same edgeId
  uint32_t sequenceId;

  /// Additional information on the edge
  std::optional<std::string> edgeDescription;

  /// True indicates that the edge is part of the base.
  /// False indicates that the edge is part of the horizon.
  bool released;

  /// The trajectory is to be communicated as a NURBS and is defined
  /// in chapter 6.4 Trajectory segments are from the point where
  /// the AGV starts to enter the edge until the point where it
  /// reports that the next node was traversed.
  std::optional<Trajectory> trajectory;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_EDGESTATE_HPP_
