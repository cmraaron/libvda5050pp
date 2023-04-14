// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 LoadDimensions
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_LOADDIMENSIONS_HPP_
#define INCLUDE_VDA5050PP_MODEL_LOADDIMENSIONS_HPP_

#include <optional>

namespace vda5050pp {
/// Dimensions of the load’s bounding box in meters.
struct LoadDimensions {
  /// [m] Absolute length of the load’s bounding box.
  double length;

  /// [m] Absolute width of the load’s bounding box.
  double width;

  /// [m] Absolute height of the load’s bounding box.
  /// Optional: Set value only if known.
  std::optional<double> height;

  ///
  ///\brief Check if two LoadDimensions are equal
  ///
  /// used to interact with stl
  ///\param other the LoadDimension to compare to
  ///\return true equal
  ///\return false not equal
  ///
  constexpr bool operator==(const LoadDimensions &other) const noexcept(true) {
    if (this->length != other.length) return false;
    if (this->width != other.width) return false;
    if (this->height != other.height) return false;
    return true;
  }

  ///
  ///\brief Check if two LoadDimensions are not equal
  ///
  /// used to interact with stl
  ///\param other the LoadDimension to compare to
  ///\return true not equal
  ///\return false equal
  ///
  constexpr bool operator!=(const LoadDimensions &other) const noexcept(true) {
    return !(this->operator==(other));
  }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_LOADDIMENSIONS_HPP_
