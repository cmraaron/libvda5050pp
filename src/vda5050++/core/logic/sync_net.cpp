// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/sync_net.h"

#include <algorithm>

using namespace vda5050pp::core::logic;

void SyncNet::initTransitionID() {
  this->transition_id_ = {LogicTaskNetTypes::k_combinator_sync_transition, this->place_id_.seq};
}

SyncNet::SyncNet(std::vector<LogicTaskNetID> &&sync_ids, LogicTaskNetID self_id)
    : sync_ids_(sync_ids), place_id_(self_id) {
  initTransitionID();
}

LogicTaskNetID SyncNet::getPlaceID() const noexcept(true) { return this->place_id_; }

LogicTaskNetID SyncNet::getTransitionID() const noexcept(true) { return this->transition_id_; }

const std::vector<LogicTaskNetID> &SyncNet::getSyncIDs() const noexcept(true) {
  return this->sync_ids_;
}

void SyncNet::attachToNet(Net &composed_net) noexcept(false) {
  // Create a transition (and maybe a place) that requires one token from
  // each sync place and puts one token on a given place

  auto to_unit_weight_pair =
      [](const LogicTaskNetID &id) -> Net::TransitionSketch::SketchWeightPairT {
    return {id, 1};
  };

  if (composed_net.findPlace(this->place_id_) == nullptr) {
    composed_net.addPlace(this->place_id_, 0);
  }

  Net::TransitionSketch sketch{this->transition_id_, {}, {to_unit_weight_pair(this->place_id_)}};

  std::transform(cbegin(this->sync_ids_), cend(this->sync_ids_), std::back_inserter(sketch.ingoing),
                 to_unit_weight_pair);

  composed_net.addTransition(sketch);
  composed_net.findTransition(this->transition_id_)->autoFire();
}