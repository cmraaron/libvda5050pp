// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 ConnectionState
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_CONNECTIONSTATE_HPP_
#define INCLUDE_VDA5050PP_MODEL_CONNECTIONSTATE_HPP_

#include <ostream>

namespace vda5050pp {
enum ConnectionState {
  /// connection between AGV and broker is active.
  ONLINE,
  /// connection between AGV and broker has gone
  /// offline in a coordinated way.
  OFFLINE,
  /// The connection between AGV and
  /// broker has unexpectedly ended.
  /// (used in e.g. MQTT Last-Will-Message)
  CONNECTIONBROKEN
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param connection_state the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const ConnectionState &connection_state) {
  switch (connection_state) {
    case ConnectionState::ONLINE:
      os << "ONLINE";
      break;
    case ConnectionState::OFFLINE:
      os << "OFFLINE";
      break;
    case ConnectionState::CONNECTIONBROKEN:
      os << "CONNECTIONBROKEN";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_CONNECTIONSTATE_HPP_
