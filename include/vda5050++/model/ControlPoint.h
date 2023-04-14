// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 ControlPoint
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_CONTROLPOINT_HPP_
#define INCLUDE_VDA5050PP_MODEL_CONTROLPOINT_HPP_

#include <optional>

namespace vda5050pp {
/// ControlPoint describing a trajectory (NURBS)
struct ControlPoint {
  /// X coordinate described in the world coordinate system.
  double x;

  /// Y coordinate described in the world coordinate system.
  double y;

  /// [rad] Range: [-π ... π]
  /// Orientation of the AGV on this position of the curve. The orientation is in
  /// world coordinates. When not defined the orientation of the AGV will be
  /// tangential to the curve.
  std::optional<double> orientation;

  /// Range : (0 ... ∞)
  /// The weight with which this control point pulls on the curve. When not defined,
  /// the default will be 1.0.
  double weight;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_CONTROLPOINT_HPP_
