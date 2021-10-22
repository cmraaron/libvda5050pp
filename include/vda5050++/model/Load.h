// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Load
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_LOAD_HPP_
#define INCLUDE_VDA5050PP_MODEL_LOAD_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "vda5050++/model/BoundingBoxReference.h"
#include "vda5050++/model/LoadDimensions.h"

namespace vda5050pp {
/// VD(M)A 5050 Load
struct Load {
  /// Unique identification number of the load
  /// (e. g. barcode or RFID). Empty field
  /// if the AGV can identify the load but
  /// didn't identify the load yet.
  /// Optional if the AGV cannot identify
  /// the load
  std::optional<std::string> loadId;

  /// Type of load
  std::optional<std::string> loadType;

  /// Indicates which load handling/carrying
  /// unit of the AGV is used, e. g. in case the
  /// AGV has multiple spots/positions to carry
  /// loads. For example: “front”, “back”,
  /// “positionC1”, etc.
  /// Optional for vehicles with only one
  /// loadPosition
  std::optional<std::string> loadPosition;

  /// Point of reference for the location of the
  /// bounding box. The point of reference is
  /// always the center of the bounding box’s
  /// bottom surface (at height = 0) and is
  /// described in coordinates of the AGV’s
  /// coordinate system.
  std::optional<BoundingBoxReference> boundingBoxReference;

  /// Dimensions of the load’s bounding box in meters.
  std::optional<LoadDimensions> loadDimensions;

  /// [kg] Absolute weight of the load measured in kg
  std::optional<uint32_t> weight;

  ///
  ///\brief Check if two Loads are equal
  ///
  /// used to interact with stl
  ///\param other the Load to compare to
  ///\return true equal
  ///\return false not equal
  ///
  constexpr bool operator==(const Load &other) const noexcept(true) {
    if (this->loadId != other.loadId) return false;
    if (this->loadType != other.loadType) return false;
    if (this->weight != other.weight) return false;
    if (this->loadDimensions != other.loadDimensions) return false;
    if (this->boundingBoxReference != other.boundingBoxReference) return false;
    if (this->loadPosition != other.loadPosition) return false;
    return true;
  }

  ///
  ///\brief Check if two Loads not are equal
  ///
  /// used to interact with stl
  ///\param other the Load to compare to
  ///\return true not equal
  ///\return false equal
  ///
  constexpr bool operator!=(const Load &other) const noexcept(true) {
    return !(this->operator==(other));
  }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_LOAD_HPP_
