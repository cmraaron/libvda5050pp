// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/cancel_net.h"

#include <stdexcept>

using namespace vda5050pp::core::logic;

CancelNet::CancelNet(std::vector<std::string> &&cancel_action_ids, LogicTaskNetID pre_place_id,
                     NetManager &net_manager) noexcept(false)
    : cancel_action_ids_(std::move(cancel_action_ids)),
      pre_place_id_(pre_place_id),
      net_manager_(net_manager) {
  SeqNrT seq = this->pre_place_id_.seq + 1;
  this->self_place_id_ = {LogicTaskNetTypes::k_combinator_cancel, seq};
  this->post_place_id_ = {LogicTaskNetTypes::k_combinator_cancel_post, seq};
  this->transition_pre_id_ = {LogicTaskNetTypes::k_combinator_cancel_pre_transition, seq};
  this->transition_post_id_ = {LogicTaskNetTypes::k_combinator_cancel_post_transition, seq};
}

void CancelNet::attachToNet(Net &composed_net) noexcept(false) {
  Net partial_net;
  auto self_ptr = partial_net.addPlace(this->self_place_id_, 0);
  auto post_ptr = partial_net.addPlace(this->post_place_id_, 0);

  // <this> might not exist, when this lambda is called.
  // It is guaranteed, that net_manager is still valid (because the lambda function object is owned
  // by the manager). the cancel_action_ids have to be copied.
  self_ptr->onChange([&net_manager = this->net_manager_, c_ids = this->cancel_action_ids_](
                         const Net::PlaceT &place, Net::TokenCounterT prev) {
    if (place.getTokens() == 1 && prev == 0)
      for (const auto &id : c_ids) {
        if (net_manager.isActionActive(id)) {
          net_manager.stopAction(id);
        }
      }
  });

  Net::TransitionSketch pre_transition = {
      this->transition_pre_id_, {{this->pre_place_id_, 1}}, {{this->self_place_id_, 1}}};
  Net::TransitionSketch post_transition = {
      this->transition_post_id_, {{this->self_place_id_, 1}}, {{this->post_place_id_, 1}}};

  composed_net.merge(std::move(partial_net), {pre_transition, post_transition});

  composed_net.findTransition(this->transition_pre_id_)->autoFire();
  composed_net.findTransition(this->transition_post_id_)->autoFire();
}

LogicTaskNetID CancelNet::getPrePlaceID() const noexcept(true) { return this->pre_place_id_; }
LogicTaskNetID CancelNet::getSelfPlaceID() const noexcept(true) { return this->self_place_id_; }
LogicTaskNetID CancelNet::getPlaceID() const noexcept(true) { return this->post_place_id_; }