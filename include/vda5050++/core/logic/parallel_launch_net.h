// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the ParallelLaunchNet declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARALLEL_LAUNCH_NET_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARALLEL_LAUNCH_NET_HPP_

#include <vector>

#include "vda5050++/core/logic/partial_net.h"
#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

///
/// \brief A partial helper net used to launch multiple tasks at once
///
class ParallelLaunchNet : public PartialNet {
private:
  LogicTaskNetID launch_point_id_;
  std::vector<LogicTaskNetID> launch_ids_;

public:
  ///
  ///\brief Create a new ParallelLaunchNet, it will attach a unit transition from
  /// launch_point_id to launch_ids
  ///
  ///\param launch_point_id the starting place
  ///\param launch_ids the places to launch
  ///
  ParallelLaunchNet(LogicTaskNetID launch_point_id, std::vector<LogicTaskNetID> &&launch_ids);

  ///
  ///\brief Get the NetID of the launch point
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID getLaunchPointId() const noexcept(true);

  ///
  ///\brief Get the NetIDs of IDs to launch
  ///
  ///\return const std::vector<LogicTaskNetID>&
  ///
  const std::vector<LogicTaskNetID> &getLaunchIds() const noexcept(true);

  virtual void attachToNet(Net &composed_net) noexcept(false) override;
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_PARALLEL_LAUNCH_NET_HPP_
