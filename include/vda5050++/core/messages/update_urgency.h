// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the UpdateUrgency declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_UPDATE_URGENCY
#define INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_UPDATE_URGENCY

#include <chrono>

namespace vda5050pp::core::messages {

enum class UpdateUrgency {
  ///\brief Equivalent of no update request
  k_none,
  ///\brief Schedule the next update within the next 10 seconds
  k_low,
  ///\brief Schedule the next update within the next 1.5 seconds
  k_medium,
  ///\brief Schedule the next update within the next 10 milliseconds
  k_high,
  ///\brief Schedule the next update immediately (blocks until sent)
  k_immediate,
};

///
///\brief get the duration associated with the given UpdateUrgency
///
///\param urgency the urgency
///\return constexpr std::chrono::system_clock::duration the associated duration
///
constexpr std::chrono::system_clock::duration durationFromUpdateUrgency(
    const UpdateUrgency &urgency) {
  using namespace std::chrono_literals;

  switch (urgency) {
    case UpdateUrgency::k_none:
      return std::chrono::system_clock::duration::max();
    case UpdateUrgency::k_low:
      return 10s;
    case UpdateUrgency::k_medium:
      return 1500ms;
    case UpdateUrgency::k_high:
      return 10ms;
    case UpdateUrgency::k_immediate:
      return 0s;
    default:
      return 0s;
  }
}

}  // namespace vda5050pp::core::messages

#endif /* INCLUDE_VDA5050_2B_2B_CORE_MESSAGES_UPDATE_URGENCY */
