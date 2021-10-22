// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Node
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_NODE_HPP_
#define INCLUDE_VDA5050PP_MODEL_NODE_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "vda5050++/model/Action.h"
#include "vda5050++/model/NodePosition.h"

namespace vda5050pp {
/// VD(M)A 5050 Node
struct Node {
  /// Unique node identification.
  std::string nodeId;

  /// Number to track the sequence of nodes and edges in an
  /// order and to simplify order updates. The main purpose is
  /// to distinguish between a node which is passed more than
  /// once within one orderId. The variable sequenceId runs
  /// across all nodes and edges of the same order and is reset
  /// when a new orderId is issued.
  uint32_t sequenceId;

  /// Additional information on the node
  std::optional<std::string> nodeDescription;

  /// True indicates that the node is part of the base.
  /// False indicates that the node is part of the horizon.
  bool released;

  /// Node position Optional for vehicle-types that do not
  /// require the node position (e.g. line-guided vehicles).
  std::optional<NodePosition> nodePosition;

  /// Array of actions to be executed in node.Empty array if no
  /// actions required. An action triggered by a node will
  /// persist until changed in another node unless restricted by
  /// durationType/durationValue.
  std::vector<Action> actions;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_NODE_HPP_
