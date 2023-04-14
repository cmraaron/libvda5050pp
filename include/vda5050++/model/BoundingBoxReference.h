// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 BoundingBoxReference
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_BOUNDINGBOXREFERENCE_HPP_
#define INCLUDE_VDA5050PP_MODEL_BOUNDINGBOXREFERENCE_HPP_

#include <optional>

namespace vda5050pp {
/// Point of reference for the location of the bounding box. The point of reference is always the
/// center of the bounding box’s bottom surface (at height = 0) and is described in coordinates of
/// the AGV’s coordinate system.
struct BoundingBoxReference {
  /// x-coordinate of the point of reference.
  double x;

  /// y-coordinate of the point of reference.
  double y;

  /// z-coordinate of the point of reference.
  double z;

  /// Orientation of the loads bounding box. Important for tugger trains etc.
  std::optional<double> theta;

  ///
  ///\brief Check if two BoundingBoxReferences are equal
  ///
  /// used to interact with stl
  ///\param other the BoundingBoxReference to compare to
  ///\return true equal
  ///\return false not equal
  ///
  constexpr bool operator==(const BoundingBoxReference &other) const noexcept(true) {
    if (this->x != other.x) return false;
    if (this->y != other.y) return false;
    if (this->z != other.z) return false;
    if (this->theta != other.theta) return false;
    return true;
  }

  ///
  ///\brief Check if two BoundingBoxReferences are not equal
  ///
  /// used to interact with stl
  ///\param other the BoundingBoxReference to compare to
  ///\return true not equal
  ///\return false equal
  ///
  constexpr bool operator!=(const BoundingBoxReference &other) const noexcept(true) {
    return !(this->operator==(other));
  }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_BOUNDINGBOXREFERENCE_HPP_
