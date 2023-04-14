// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 NodePosition
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_NODEPOSITION_HPP_
#define INCLUDE_VDA5050PP_MODEL_NODEPOSITION_HPP_

#include <optional>

namespace vda5050pp {
/// Defines the position on a map in world coordinates. Each floor has its own map.
struct NodePosition {
  /// [m] X-position on the map in reference to the
  ///     world coordinate system
  double x;

  /// [m] Y-position on the map in reference to the
  ///     world coordinate system
  double y;

  /// [rad] Range : [-PI... PI]
  /// Orientation of the AGV on the node. Optional: vehicle can plan
  /// the path by itself. If defined, the AGV has to assume the theta
  /// angle on this node. If previous edge disallows rotation, the AGV
  /// must rotate on the node. If following edge has a differing
  /// orientation defined but disallows rotation, the AGV is to rotate
  /// on the node to the edges desired rotation before entering the edge
  std::optional<double> theta;

  /// [m] Indicates how exact an AGV has to drive over a node in order
  /// for it to count as traversed.
  ///
  /// If = 0: no deviation is allowed (no deviation means within the
  /// normal tolerance of the AGV manufacturer).
  ///
  /// If > 0: allowed deviationradius in meters. If the AGV passes a
  /// node within the deviation-radius, the node is considered to have
  /// been traversed.
  std::optional<double> allowedDeviationXY;

  /// [rad] Range : [0 ... Pi]
  /// Indicates how big the deviation of theta angle can be.
  /// The lowest acceptable angle is theta - allowedDevaitionTheta and
  /// the highest acceptable angle is theta + allowedDeviationTheta.
  std::optional<double> allowedDeviationTheta;

  /// Unique identification of the map in which the position is
  /// referenced. Each map has the same origin of coordinates. When an
  /// AGV uses an elevator, e. g. leading from a departure floor to a
  /// target floor, it will disappear off the map of the departure floor
  /// and spawn in the related lift node on the map of the target floor.
  std::string mapId;

  /// Additional information on the map
  std::optional<std::string> mapDescription;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_NODEPOSITION_HPP_
