// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/parallel_launch_net.h"

#include <algorithm>

using namespace vda5050pp::core::logic;

ParallelLaunchNet::ParallelLaunchNet(LogicTaskNetID launch_point_id,
                                     std::vector<LogicTaskNetID> &&launch_ids)
    : launch_point_id_(launch_point_id), launch_ids_(launch_ids) {}

LogicTaskNetID ParallelLaunchNet::getLaunchPointId() const noexcept(true) {
  return this->launch_point_id_;
}

const std::vector<LogicTaskNetID> &ParallelLaunchNet::getLaunchIds() const noexcept(true) {
  return this->launch_ids_;
}

void ParallelLaunchNet::attachToNet(Net &composed_net) noexcept(false) {
  // Add a transition which takes one token from the launch point
  // and adds one token to all places with one of the launchIDs

  auto to_unit_weight_pair =
      [](const LogicTaskNetID &id) -> Net::TransitionSketch::SketchWeightPairT {
    return {id, 1};
  };

  LogicTaskNetID id = {LogicTaskNetTypes::k_combinator_parallel, this->launch_point_id_.seq};

  Net::TransitionSketch sketch{id, {to_unit_weight_pair(this->launch_point_id_)}, {}};

  std::transform(cbegin(this->launch_ids_), cend(this->launch_ids_),
                 std::back_inserter(sketch.outgoing), to_unit_weight_pair);

  composed_net.addTransition(sketch);
  composed_net.findTransition(id)->autoFire();
}