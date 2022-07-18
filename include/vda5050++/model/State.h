// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 State
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_STATE_HPP_
#define INCLUDE_VDA5050PP_MODEL_STATE_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/ActionState.h"
#include "vda5050++/model/BatteryState.h"
#include "vda5050++/model/EdgeState.h"
#include "vda5050++/model/Error.h"
#include "vda5050++/model/Header.h"
#include "vda5050++/model/Info.h"
#include "vda5050++/model/Load.h"
#include "vda5050++/model/NodeState.h"
#include "vda5050++/model/OperatingMode.h"
#include "vda5050++/model/SafetyState.h"
#include "vda5050++/model/Velocity.h"

namespace vda5050pp {
/// VD(M)A 5050 State
struct State {
  /// Message header
  Header header;

  /// Unique order identification of the current order or the
  /// previous finished order. The orderId is kept until a new
  /// order is received. Empty string ("") if no previous
  /// orderId is available.
  std::string orderId;

  /// Order Update Identification to identify that an order
  /// update has been accepted by the AGV. “0” if no previous
  /// orderUpdateId is available.
  uint32_t orderUpdateId;

  /// Unique ID of the zone set that the AGV currently uses for path
  /// planning. Must be the same as the one used in the order, otherwise
  /// the AGV has to reject the order.
  /// Optional: If the AGV does not use zones,
  /// this field can be omitted.
  std::optional<std::string> zoneSetId;

  /// nodeId of last reached node or, if AGV is currently on a
  /// node, current node (e.g. „node7”).
  /// Empty string ("") if no lastNodeId is available.
  std::string lastNodeId;

  /// sequenceId of the last reached node or, if the AGV is
  /// currently on a node, sequenceId of current node.
  /// “0” if no lastNodeSequenceId is available.
  uint32_t lastNodeSequenceId;

  /// Array of nodeStateObjects that need to be traversed for
  /// fulfilling the order (empty list if idle)
  std::vector<NodeState> nodeStates;

  /// Array of edgeStateObjects that need to be traversed for
  /// fulfilling the order (empty list if idle)
  std::vector<EdgeState> edgeStates;

  /// Current position of the AGV on the map.
  /// Optional: Can only be omitted for AGVs without the
  /// capability to localize themselves, e.g. line guided AGVs.
  std::optional<AGVPosition> agvPosition;

  /// AGV's velocity in vehicle coordinates
  std::optional<Velocity> velocity;

  /// Loads that are currently handled by the AGV.
  /// Optional: If AGV cannot determine load state, leave the
  /// array out of the state. If the AGV can determine the load
  /// state, but the array is empty, the AGV is considered
  /// unloaded.
  std::vector<Load> loads;

  /// True: indicates that the AGV is driving and/or rotating. Other
  /// movements of the AGV (e.g. lift movements) are not included here.
  /// False: indicates that the AGV is neither driving nor rotating
  bool driving;

  /// True: AGV is currently in a paused state, either because of the push
  /// of a physical button on the AGV or because of an instantAction.
  /// The AGV can resume the order.
  /// False: The AGV is currently not in a paused state
  bool paused;

  /// True: AGV is almost at the end of the base and will reduce
  /// speed if no new base is transmitted. Trigger for MC to
  /// send new base.
  /// False: no base update required
  bool newBaseRequested;

  /// [m] Used by line guided vehicles to indicate the distance
  /// it has been driving past the „lastNodeIdId“.
  /// Distance is in meters.
  std::optional<double> distanceSinceLastNode;

  /// Contains a list of the current actions and the actions
  /// which are yet to be finished. This may include actions
  /// from previous nodes that are still in progress. When an
  /// action is completed, an updated state message is published
  /// with actionStatus set to finished and if applicable with
  /// the corresponding resultDescription.
  /// The action state is kept until a new order is received.
  std::vector<ActionState> actionStates;

  /// Contains all battery related information.
  BatteryState batteryState;

  /// Enum {automatic, semi-automatic, manual, service, teach-in}
  /// For additional information see chapter 6.11
  OperatingMode operatingMode;

  /// Array of error - objects. All active errors of the AGV
  /// should be in the list. An empty array indicates that the
  /// AGV has no active errors.
  std::vector<Error> errors;

  /// Array of info-objects.An empty array indicates that the AGV
  /// has no information. This should only be used for visualization
  /// or debugging – it must not be used for logic in master control.
  std::vector<Info> informations;

  /// Contains all safetyrelated information.
  SafetyState safetyState;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_STATE_HPP_
