// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the definition for the VD(M)A 5050 Order
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ORDER_HPP_
#define INCLUDE_VDA5050PP_MODEL_ORDER_HPP_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "vda5050++/model/Edge.h"
#include "vda5050++/model/Header.h"
#include "vda5050++/model/Node.h"

namespace vda5050pp {
/// VD(M)A 5050 Order
struct Order {
  /// Message header
  Header header;

  /// Order identification. This is to be used to identify
  /// multiple order messages that belong to the same order.
  std::string orderId;

  /// orderUpdate identification. Is unique per orderId. If an order
  /// update is rejected, this field is to be passed in the rejection
  /// message
  uint32_t orderUpdateId;

  /// Unique identifier of the zone set that the AGV has to use for
  /// navigation or that was used by MC for planning.
  std::optional<std::string> zoneSetId;

  /// Array of nodes objects to be traversed for fulfilling the order.
  /// One node is enough for a valid order. Leave edge list empty for
  /// that case.
  std::vector<Node> nodes;

  /// Array of edges to be traversed for fulfilling the order. May
  /// be empty in case only one node is used for an order.
  std::vector<Edge> edges;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ORDER_HPP_
