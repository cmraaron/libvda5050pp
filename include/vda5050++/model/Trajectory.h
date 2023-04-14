// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 Trajectory
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_TRAJECTORY_HPP_
#define INCLUDE_VDA5050PP_MODEL_TRAJECTORY_HPP_

#include <vector>

#include "vda5050++/model/ControlPoint.h"

namespace vda5050pp {

/// Points defining a spline. Theta allows holonomic vehicles to rotate along the trajecotry.
struct Trajectory {
  /// Range: [1 … infinity) Defines the number of control
  /// points that influence any given point on the curve.
  /// Increasing the degree increases continuity.
  /// If not defined, the default value is 1.
  double degree;

  /// Range : [0.0 … 1.0] Sequence of parameter values that
  /// determines where and how the control points affect
  /// the NURBS curve.
  /// knotVector has size of
  /// number of control points + degree + 1.
  std::vector<double> knotVector;

  /// List of controlPoint objects defining the
  /// control points of the nurbs, which includes
  /// the beginning and end point.
  std::vector<ControlPoint> controlPoints;
};

}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_TRAJECTORY_HPP_
