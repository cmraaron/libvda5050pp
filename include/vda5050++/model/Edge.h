// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Edge
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_EDGE_HPP_
#define INCLUDE_VDA5050PP_MODEL_EDGE_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "vda5050++/model/Action.h"
#include "vda5050++/model/OrientationType.h"
#include "vda5050++/model/Trajectory.h"

namespace vda5050pp {

/// Directional connection between two nodes
struct Edge {
  /// Unique edge identification
  std::string edgeId;

  /// Id to track the sequence of nodes and edges in an order and to
  /// simplify order updates.The variable sequenceId runs across
  /// all nodes and edges of the same order and is reset when a new
  /// orderId is issued.
  uint32_t sequenceId;

  /// Additional information on the edge
  std::optional<std::string> edgeDescription;

  /// True indicates that the edge is part of the base.
  /// False indicates that the edge is part of the horizon.
  bool released;

  /// nodeID of startNode
  std::string startNodeId;

  /// nodeID of endNode
  std::string endNodeId;

  /// [m/s] Permitted maximum speed on the edge. Speed is defined
  /// by the fastest point of the vehicle.
  std::optional<double> maxSpeed;

  /// [m] Permitted maximal height of the load handling device
  /// on the edge.
  std::optional<double> maxHeight;

  /// [m] Permitted minimal height of the load handling device
  /// on the edge.
  std::optional<double> minHeight;

  /// [rad] Orientation of the AGV on the edge relative to the
  /// tangential direction of the edge (for holonomic vehicles with
  /// more than one driving direction).
  /// Example: orientation Pi/2 rad will lead to a rotation
  /// of 90 degrees.
  ///
  /// If AGV starts in different orientation, rotate the vehicle on
  /// the edge to the desired orientation if rotationAllowed is set
  /// to “true”. If rotationAllowed is “false", rotate before
  /// entering the edge. If that is not possible, reject the order.
  ///
  /// If a trajectory with orientation is defined, follow the
  /// trajectories orientation. If a trajectory without orientation
  /// and the orientation field here is defined, apply the
  /// orientation to the tangent of the trajectory.
  std::optional<double> orientation;

  /// Enum {GLOBAL, TANGENTIAL}
  /// "GLOBAL"- relative to the global project specific map coordinate system
  /// "TANGENTIAL"- tangential to the edge.
  /// If not defined, the default value is "TANGENTIAL".
  OrientationType orientationType = OrientationType::TANGENTIAL;

  /// Sets direction at junctions for line-guided or wire-guided
  /// vehicles, to be defined initially (vehicle individual).
  /// Example: left, right, straight, 433MHz
  std::optional<std::string> direction;

  /// “true”: rotation is allowed on the edge.
  /// “false”: rotation is not allowed on the edge.
  /// Optional: Default to "false"
  std::optional<bool> rotationAllowed;

  /// [rad/s] Maximum rotation speed
  std::optional<double> maxRotationSpeed;

  /// Trajectory JSON-object for this edge as a NURBS.
  /// Defines the curve on which the AGV should move between
  /// startNode and endNode.
  /// Optional: Can be omitted if AGV cannot process trajectories
  /// or if AGV plans its own trajectory.
  std::optional<Trajectory> trajectory;

  /// [m] Length of the path from startNode to endNode.
  /// Optional: This value is used by line-guided AGVs to decrease
  /// their speed before reaching a stop position.
  std::optional<double> length;

  /// Array of actions to be executed on the edge.
  /// An action triggered by an edge will only be active for the
  /// time that the AGV is traversing the edge which triggered
  /// the action. When the AGV leaves the edge, the action will stop
  /// and the state before entering the edge will be restored.
  std::vector<Action> actions;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_EDGE_HPP_
