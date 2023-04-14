// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the SyncNet declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_SYNC_NET_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_SYNC_NET_HPP_

#include <vector>

#include "vda5050++/core/logic/partial_net.h"
#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

///
/// \brief A partial helper net used to wait for multiple "ptn threads"
///
///
class SyncNet : public PartialNet {
private:
  LogicTaskNetID transition_id_;
  std::vector<LogicTaskNetID> sync_ids_;
  LogicTaskNetID place_id_;

  void initTransitionID();

public:
  ///
  ///\brief Construct a new synchronizer component with explicit id
  ///       NOTE: IF self_id does not exist, it will be created
  ///
  /// this->transition_id_.seq will be this->place_id_.seq
  ///
  ///\param sync_ids all ids of the places to sync
  ///\param self_id the id to use for the sync place
  ///
  SyncNet(std::vector<LogicTaskNetID> &&sync_ids, LogicTaskNetID self_id);

  ///
  ///\brief get the net ID
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID getPlaceID() const noexcept(true);

  ///
  ///\brief Get the Transition ID
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID getTransitionID() const noexcept(true);

  ///
  ///\brief Get the IDs of all sync places
  ///
  ///\return const std::vector<LogicTaskNetID>&
  ///
  const std::vector<LogicTaskNetID> &getSyncIDs() const noexcept(true);

  virtual void attachToNet(Net &composed_net) noexcept(false) override;
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_SYNC_NET_HPP_
