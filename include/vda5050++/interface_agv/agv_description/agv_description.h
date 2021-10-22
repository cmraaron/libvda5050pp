// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// TODO: Doc

#ifndef INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_AGV_DESCRIPTION_HPP_
#define INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_AGV_DESCRIPTION_HPP_

#include <cstdint>
#include <optional>
#include <set>
#include <string>

#include "vda5050++/interface_agv/agv_description/action_declaration.h"
#include "vda5050++/interface_agv/agv_description/battery.h"
#include "vda5050++/interface_agv/agv_description/navigation.h"

namespace vda5050pp::interface_agv::agv_description {

/// \brief The library configuration
struct AGVDescription {
  /// Use a UUID v5 for unique AGV ID
  std::string agv_id;
  /// Manufacturer string (message header)
  std::string manufacturer;

  /// Serial number (message header)
  std::string serial_number;

  /// An additional description of the AGV
  std::string description;

  // TODO: Footprint

  /// Battery Information
  Battery battery;

  /// Navigation mode of the AGV
  Navigation navigation;

  /// [kg] Weight of the AGV without load
  double weight;

  ///
  /// \brief all supported actions
  ///
  /// Note: only actions explicitly handled by the AGV must be defined here
  ///       (cancelOrder for example must not be defined)
  ///
  /// if available, the library will scan each action and check
  /// it against this set. Unsupported actions will be automatically
  /// reported to the MC. \n
  /// Empty set -> no actions available \n
  /// nullopt -> do not validate actions \n
  std::optional<std::set<ActionDeclaration>> supported_actions;
};

}  // namespace vda5050pp::interface_agv::agv_description
#endif  // INCLUDE_VDA5050PP_INTERFACE_AGV_AGV_DESCRIPTION_AGV_DESCRIPTION_HPP_
