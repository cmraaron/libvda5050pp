// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the State declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE
#define INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE

#include <cstdint>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "vda5050++/model/Action.h"
#include "vda5050++/model/ActionState.h"
#include "vda5050++/model/Edge.h"
#include "vda5050++/model/EdgeState.h"
#include "vda5050++/model/Node.h"
#include "vda5050++/model/NodeState.h"
#include "vda5050++/model/State.h"

namespace vda5050pp::core::state {

///
///\brief Aggregation of the whole state of the library.
///
/// The vda5050pp::interface_agv::Handle will maintain an object of this kind.
///
struct State {
  mutable std::shared_mutex mutex;

  ///
  ///\brief Read-only lock the state
  ///
  ///\return std::shared_lock<std::shared_mutex>&&
  ///
  [[nodiscard]] inline std::shared_lock<std::shared_mutex> acquireShared() const {
    return std::shared_lock(this->mutex);
  }

  ///
  ///\brief RW lock the state
  ///
  ///\return std::scoped_lock<std::shared_mutex>&&
  ///
  [[nodiscard]] inline std::scoped_lock<std::shared_mutex> acquire() {
    return std::scoped_lock(this->mutex);
  }

  ///
  ///\brief The state as defined in VD(M)A 5050
  ///
  /// Simple information will be stored in it, but more complex like actions nodes and edges
  /// are maintained inside of the vda5050pp::core::state::State class due to better
  /// accessibility
  vda5050pp::State state;

  ///
  ///\brief Holds all static actions of the order
  ///
  std::map<std::string, vda5050pp::Action, std::less<>> action_by_id;

  ///
  ///\brief Holds all instant actions received during the current order
  ///
  std::map<std::string, vda5050pp::Action, std::less<>> instant_action_by_id;

  ///
  ///\brief Holds all action states
  ///
  std::map<std::string, vda5050pp::ActionState, std::less<>> action_state_by_id;

  ///
  ///\brief Holds all relevant edges of the current order
  ///
  std::map<uint32_t, vda5050pp::Edge> edge_by_seq;

  ///
  ///\brief Holds all edge states
  ///
  std::map<uint32_t, vda5050pp::EdgeState> edge_state_by_seq;

  ///
  ///\brief Holds all relevant nodes of the current order
  ///
  std::map<uint32_t, vda5050pp::Node> node_by_seq;

  ///
  ///\brief Holds all node states
  ///
  std::map<uint32_t, vda5050pp::NodeState> node_state_by_seq;

  ///
  ///\brief The current highest sequence id of the Order's graph (base only)
  ///
  uint32_t graph_base_seq_id = 0;

  ///
  ///\brief The sequence id of the graph element to be interpreted next
  ///
  uint32_t graph_next_interpreted_seq_id_ = 0;

  ///
  ///\brief Sequence id for the connection topic
  ///
  uint32_t connection_seq_id = 0;

  ///
  ///\brief Sequence id for the instant actions topic
  ///
  uint32_t instant_actions_seq_id = 0;

  ///
  ///\brief Sequence id for the order topic
  ///
  uint32_t order_seq_id = 0;

  ///
  ///\brief Sequence id for the state topic
  ///
  uint32_t state_seq_id = 0;

  ///
  ///\brief Sequence id for the visualization topic
  ///
  uint32_t visualization_seq_id = 0;
};

}  // namespace vda5050pp::core::state

#endif /* INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE */
