// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/dangling_net.h"

using namespace vda5050pp::core::logic;

DanglingNet::DanglingNet(LogicTaskNetID invoke_place)
    : DanglingNet(invoke_place,
                  {LogicTaskNetTypes::k_combinator_dangling_begin, invoke_place.seq}) {}
DanglingNet::DanglingNet(LogicTaskNetID invoke_place, LogicTaskNetID begin_place)
    : begin_place_(begin_place),
      initial_transition_({LogicTaskNetTypes::k_combinator_dangling_transition, begin_place.seq}),
      invoke_place_(invoke_place) {}

void DanglingNet::attachToNet(Net &composed_net) noexcept(false) {
  composed_net.addPlace(this->begin_place_, 1);
  composed_net.addTransition(
      {this->initial_transition_, {{this->begin_place_, 1}}, {{this->invoke_place_, 1}}});
}

LogicTaskNetID DanglingNet::getBeginPlace() const noexcept(true) { return this->begin_place_; }

LogicTaskNetID DanglingNet::getInvokePlace() const noexcept(true) { return this->invoke_place_; }

LogicTaskNetID DanglingNet::getTransition() const noexcept(true) {
  return this->initial_transition_;
}