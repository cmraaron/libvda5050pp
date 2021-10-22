// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 NodeState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_NODESTATE_HPP_
#define INCLUDE_VDA5050PP_MODEL_NODESTATE_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "vda5050++/model/NodePosition.h"

namespace vda5050pp {
/// VD(M)A 5050 NodeState
struct NodeState {
  /// Unique node identification
  std::string nodeId;

  /// sequenceId to discern multiple nodes with same nodeId
  uint32_t sequenceId;

  /// Additional information on the node
  std::optional<std::string> nodeDescription;

  /// Node position.The object is defined in chapter 6.6
  /// Optional: MC has this information. Can be sent
  /// additionally, e. g. for debugging purposes.
  std::optional<NodePosition> nodePosition;

  /// true indicates that the node is part of the base.
  /// false indicates that the node is part of the horizon.
  bool released;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_NODESTATE_HPP_
