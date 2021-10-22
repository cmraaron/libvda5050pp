// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 ActionStatus
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_ACTIONSTATUS_HPP_
#define INCLUDE_VDA5050PP_MODEL_ACTIONSTATUS_HPP_

#include <ostream>

namespace vda5050pp {
enum ActionStatus {
  /// Action was received by AGV but the node where it triggers was
  /// not yet reached or the edge where it is active was not
  /// yet entered.
  WAITING,

  /// Action was triggered, preparatory measures are initiated.
  INITIALIZING,

  /// The action is paused because of a pause instantAction or
  /// external trigger (pause button on AGV)
  RUNNING,

  /// The action is paused because of a pause instantAction or external
  /// trigger (pause button on AGV)
  PAUSED,

  /// The action is finished. A result is reported via the
  /// resultDescription
  FINISHED,

  /// Action could not be finished for whatever reason.
  FAILED
};

///
///\brief Write the enum-value to an ostream
///
///\param os the stream
///\param action_status the enum
///\return constexpr std::ostream&
///
constexpr std::ostream &operator<<(std::ostream &os, const ActionStatus &action_status) {
  switch (action_status) {
    case ActionStatus::WAITING:
      os << "WAITING";
      break;
    case ActionStatus::INITIALIZING:
      os << "INITIALIZING";
      break;
    case ActionStatus::RUNNING:
      os << "RUNNING";
      break;
    case ActionStatus::PAUSED:
      os << "PAUSED";
      break;
    case ActionStatus::FINISHED:
      os << "FINISHED";
      break;
    case ActionStatus::FAILED:
      os << "FAILED";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_ACTIONSTATUS_HPP_
