// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the declaration of the DanglinNet
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DANGLING_NET
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DANGLING_NET

#include "vda5050++/core/logic/partial_net.h"
#include "vda5050++/core/logic/types.h"

namespace vda5050pp::core::logic {

class DanglingNet : public PartialNet {
private:
  LogicTaskNetID begin_place_;
  LogicTaskNetID initial_transition_;
  LogicTaskNetID invoke_place_;

public:
  explicit DanglingNet(LogicTaskNetID invoke_place);
  explicit DanglingNet(LogicTaskNetID invoke_place, LogicTaskNetID begin_place);

  void attachToNet(Net &composed_net) noexcept(false) override;

  LogicTaskNetID getBeginPlace() const noexcept(true);

  LogicTaskNetID getInvokePlace() const noexcept(true);

  LogicTaskNetID getTransition() const noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_DANGLING_NET */
