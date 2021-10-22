// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.

// This file contains the definition for the VD(M)A 5050 Header
// SEE: Schnittstelle zur Kommunikation zwischen Fahrerlosen Transportfahr-
// zeugen (FTF) und einer Leitsteuerung
// VDA 5050 Version 2.0, Mai 2020

#ifndef INCLUDE_VDA5050PP_MODEL_HEADER_HPP_
#define INCLUDE_VDA5050PP_MODEL_HEADER_HPP_

#include <chrono>
#include <cstdint>
#include <string>

namespace vda5050pp {
/// The definition of a vda5050pp header sent with each message
struct Header {
  /// header ID of the message. The headerId is defined per topic and incremented by
  /// 1 with each sent(but not necessarily received) message.
  uint32_t headerId;

  /// Timestamp after ISO8601 in the format YYYY-MM-DDTHH:mm:ss.ssZ
  /// (e.g.“2017-04-15T11:40:03.12Z”)
  ///
  /// Note: This structure will not be serialized directly, so
  /// a time_point of the system clock is the most convinient datatype
  std::chrono::time_point<std::chrono::system_clock> timestamp;

  /// Version of the protocol[Major].[Minor].[Patch](e.g .1.3.2)
  std::string version;

  /// Manufacturer of the AGV
  std::string manufacturer;

  /// Serial Number of the AGV
  std::string serialNumber;

  constexpr bool operator==(const Header &other) const {
    if (headerId != other.headerId) return false;
    if (timestamp != other.timestamp) return false;
    if (version != other.version) return false;
    if (manufacturer != other.manufacturer) return false;
    if (serialNumber != other.serialNumber) return false;
    return true;
  }
  constexpr bool operator!=(const Header &other) const { return !this->operator==(other); }
};
}  // namespace vda5050pp
#endif  // INCLUDE_VDA5050PP_MODEL_HEADER_HPP_
