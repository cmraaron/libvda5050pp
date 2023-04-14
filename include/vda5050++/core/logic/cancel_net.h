// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the CancelNet class
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CANCEL_NET_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CANCEL_NET_HPP_

#include <vector>

#include "vda5050++/core/logic/net_manager.h"
#include "vda5050++/core/logic/partial_net.h"
#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

///
/// \brief A partial helper net used to cancel a certain task on traversal
///
class CancelNet : public PartialNet {
private:
  std::vector<std::string> cancel_action_ids_;

  LogicTaskNetID pre_place_id_;
  LogicTaskNetID self_place_id_;
  LogicTaskNetID post_place_id_;
  LogicTaskNetID transition_pre_id_;
  LogicTaskNetID transition_post_id_;

  NetManager &net_manager_;

public:
  CancelNet(std::vector<std::string> &&cancel_action_ids, LogicTaskNetID pre_place_id,
            NetManager &net_manager) noexcept(false);

  virtual void attachToNet(Net &composed_net) noexcept(false) override;

  /// \brief Get the Place before the cancel place
  LogicTaskNetID getPrePlaceID() const noexcept(true);
  /// \brief Get the cancel place
  LogicTaskNetID getSelfPlaceID() const noexcept(true);
  /// \brief Get the end place
  LogicTaskNetID getPlaceID() const noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CANCEL_NET_HPP_
