// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Connection
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_CONNECTION_HPP_
#define INCLUDE_VDA5050PP_MODEL_CONNECTION_HPP_

#include "vda5050++/model/ConnectionState.h"
#include "vda5050++/model/Header.h"

namespace vda5050pp {
/// A message containing connection info
struct Connection {
  /// Message header
  Header header;

  /// Enum{ONLINE, OFFLINE, CONNECTIONBROKEN}
  /// ONLINE:  connection between AGV and broker is active.
  /// OFFLINE: connection between AGV and broker has gone
  ///          offline in a coordinated way.
  /// CONNECTIONBROKEN: The connection between AGV and
  ///                   broker has unexpectedly ended.
  ///                   (used in e.g. MQTT Last-Will-Message)
  ConnectionState connectionState;
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_CONNECTION_HPP_
