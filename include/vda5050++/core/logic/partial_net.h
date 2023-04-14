// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the PartialNet interface class
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARTIAL_NET_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARTIAL_NET_HPP_

#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

///
///\brief Interface class for classes which manage parts of the global petri net
///
class PartialNet {
public:
  virtual ~PartialNet() = default;

  ///
  ///\brief Attach the own partial net to the composed global petri net
  ///
  ///\param composed_net the global composed net
  ///
  virtual void attachToNet(Net &composed_net) noexcept(false) = 0;
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARTIAL_NET_HPP_
