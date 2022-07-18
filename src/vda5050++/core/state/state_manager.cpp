// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/state/state_manager.h"

#include <algorithm>
#include <stdexcept>

using namespace vda5050pp::core::state;

static vda5050pp::ActionState state_from_action(const vda5050pp::Action &a) {
  return {a.actionId, std::make_optional(a.actionType), a.actionDescription,
          vda5050pp::ActionStatus::WAITING, std::nullopt};
}

static vda5050pp::NodeState state_from_node(const vda5050pp::Node &n) {
  return {n.nodeId, n.sequenceId, n.nodeDescription, n.nodePosition, n.released};
}

static vda5050pp::EdgeState state_from_edge(const vda5050pp::Edge &e) {
  return {e.edgeId, e.sequenceId, e.edgeDescription, e.released, e.trajectory};
}

template <typename MapT, typename PredT> static void erase_if(MapT &map, const PredT &pred) {
  auto it = begin(map);
  while (it != end(map)) {
    if (pred(*it)) {
      it = map.erase(it);
    } else {
      it++;
    }
  }
}

StateManager::StateManager() noexcept(true) {
  this->state_.state.actionStates = {};
  this->state_.state.agvPosition = std::nullopt;
  this->state_.state.batteryState = {};
  this->state_.state.distanceSinceLastNode = 0;
  this->state_.state.driving = false;
  this->state_.state.edgeStates = {};
  this->state_.state.errors = {};
  this->state_.state.informations = {};
  this->state_.state.lastNodeId = "";
  this->state_.state.lastNodeSequenceId = 0;
  this->state_.state.loads = {};
  this->state_.state.newBaseRequested = false;
  this->state_.state.nodeStates = {};
  this->state_.state.operatingMode = vda5050pp::OperatingMode::AUTOMATIC;
  this->state_.state.orderId = "";
  this->state_.state.orderUpdateId = 0;
  this->state_.state.paused = false;
  this->state_.state.safetyState = {};
  this->state_.state.velocity = std::nullopt;
  this->state_.state.zoneSetId = std::nullopt;

  this->state_.action_by_id = {};
  this->state_.action_state_by_id = {};
  this->state_.connection_seq_id = 0;
  this->state_.edge_by_seq = {};
  this->state_.edge_state_by_seq = {};
  this->state_.graph_base_seq_id = 0;
  this->state_.graph_next_interpreted_seq_id_ = 0;
  this->state_.instant_action_by_id = {};
  this->state_.instant_actions_seq_id = 0;
  this->state_.node_by_seq = {};
  this->state_.node_state_by_seq = {};
  this->state_.order_seq_id = 0;
  this->state_.state_seq_id = 0;
  this->state_.visualization_seq_id = 0;
}

State &StateManager::getStateUnsafe() noexcept(true) { return this->state_; }

vda5050pp::Action StateManager::getActionById(const std::string &id) const noexcept(false) {
  auto lock = this->state_.acquireShared();

  auto it = this->state_.action_by_id.find(id);

  if (it == end(this->state_.action_by_id)) {
    throw std::invalid_argument("No action associated with id: " + id);
  }

  return it->second;
}

void StateManager::setActionStatus(const std::string &id,
                                   vda5050pp::ActionStatus status) noexcept(false) {
  auto lock = this->state_.acquire();

  auto it = this->state_.action_state_by_id.find(id);

  if (it == end(this->state_.action_state_by_id)) {
    throw std::invalid_argument("No action associated with id: " + id);
  }

  it->second.actionStatus = status;
}

void StateManager::setActionResult(const std::string &id,
                                   const std::string &result) noexcept(false) {
  auto lock = this->state_.acquire();

  auto it = this->state_.action_state_by_id.find(id);

  if (it == end(this->state_.action_state_by_id)) {
    throw std::invalid_argument("No action associated with id: " + id);
  }

  it->second.resultDescription = std::make_optional(result);
}

void StateManager::unsetActionResult(const std::string &id) noexcept(false) {
  auto lock = this->state_.acquire();

  auto it = this->state_.action_state_by_id.find(id);

  if (it == end(this->state_.action_state_by_id)) {
    throw std::invalid_argument("No action associated with id: " + id);
  }

  it->second.resultDescription.reset();
}

std::string StateManager::getGraphIdBySeqIdAcquired(uint32_t seq_id) const noexcept(false) {
  if (this->state_.node_by_seq.size() + this->state_.edge_by_seq.size() < seq_id) {
    throw std::invalid_argument("Invalid seq_id");
  }

  if (seq_id % 2 == 0) {
    return this->state_.node_by_seq.at(seq_id).nodeId;
  } else {
    return this->state_.edge_by_seq.at(seq_id).edgeId;
  }
}

std::string StateManager::getGraphIdBySeqId(uint32_t seq_id) const noexcept(false) {
  auto lock = this->state_.acquireShared();

  return this->getGraphIdBySeqIdAcquired(seq_id);
}

void StateManager::appendOrder(const vda5050pp::Order &order) noexcept(true) {
  auto lock = this->state_.acquire();

  auto apply_snd = [](const auto &f) { return [&f](const auto &pair) { return f(pair.second); }; };
  auto is_horizon = [](const auto &elem) { return !elem.released; };

  this->state_.state.orderId = order.orderId;
  this->state_.state.orderUpdateId = order.orderUpdateId;

  // Clear Horizon
  erase_if(this->state_.edge_by_seq, apply_snd(is_horizon));
  erase_if(this->state_.edge_state_by_seq, apply_snd(is_horizon));
  erase_if(this->state_.node_by_seq, apply_snd(is_horizon));
  erase_if(this->state_.node_state_by_seq, apply_snd(is_horizon));

  // Add all edges and base actions
  for (const auto &edge : order.edges) {
    this->state_.edge_by_seq[edge.sequenceId] = edge;
    this->state_.edge_state_by_seq[edge.sequenceId] = state_from_edge(edge);

    // skip horizon
    if (is_horizon(edge)) {
      continue;
    }

    for (const auto &a : edge.actions) {
      if (this->state_.action_state_by_id.find(a.actionId) !=
          this->state_.action_state_by_id.cend()) {
        continue;
      }

      // add all found actions
      this->state_.action_state_by_id[a.actionId] = state_from_action(a);
      this->state_.action_by_id[a.actionId] = a;
    }
  }

  // Add all nodes and base actions (also add increment the graph_base_seq_id_)
  for (const auto &node : order.nodes) {
    this->state_.node_by_seq[node.sequenceId] = node;
    this->state_.node_state_by_seq[node.sequenceId] = state_from_node(node);

    // Skip horizon
    if (is_horizon(node)) {
      continue;
    }

    this->state_.graph_base_seq_id = std::max(this->state_.graph_base_seq_id, node.sequenceId);

    for (const auto &a : node.actions) {
      if (this->state_.action_state_by_id.find(a.actionId) !=
          this->state_.action_state_by_id.cend()) {
        continue;
      }
      // add all found actions
      this->state_.action_state_by_id[a.actionId] = state_from_action(a);
      this->state_.action_by_id[a.actionId] = a;
    }
  }
}

void StateManager::setOrder(const vda5050pp::Order &order) noexcept(true) {
  clearOrder();
  appendOrder(order);

  // set current node to be the first one (AGV is already on this node, otherwise the order was
  // rejected)
  if (!order.nodes.empty()) {
    this->setLastNodeReached(0);
  }
}

void StateManager::clearOrder() noexcept(true) {
  auto lock = this->state_.acquire();

  this->state_.action_by_id.clear();
  this->state_.action_state_by_id.clear();
  this->state_.edge_by_seq.clear();
  this->state_.edge_state_by_seq.clear();
  this->state_.instant_action_by_id.clear();
  this->state_.node_by_seq.clear();
  this->state_.node_state_by_seq.clear();
  this->state_.graph_base_seq_id = 0;
  this->state_.graph_next_interpreted_seq_id_ = 0;

  this->state_.state.distanceSinceLastNode = 0;
  this->state_.state.errors.clear();
  this->state_.state.informations.clear();
  this->state_.state.newBaseRequested = false;
  this->state_.state.orderId = "";
  this->state_.state.orderUpdateId = 0;
}

void StateManager::insertInstantActions(const vda5050pp::InstantActions &instant_actions) noexcept(
    true) {
  auto lock = this->state_.acquire();

  for (const auto &a : instant_actions.instantActions) {
    this->state_.action_state_by_id[a.actionId] = state_from_action(a);
    this->state_.instant_action_by_id[a.actionId] = a;
  }

  this->state_.instant_actions_seq_id = instant_actions.header.headerId;
}

void StateManager::setZoneSetId(const std::string &id) noexcept(true) {
  auto lock = state_.acquire();
  this->state_.state.zoneSetId = id;
}

void StateManager::unsetZoneSetId() noexcept(true) {
  auto lock = state_.acquire();
  this->state_.state.zoneSetId.reset();
}

void StateManager::setLastNodeReached(uint32_t seq_id) noexcept(false) {
  auto lock = this->state_.acquire();

  if (seq_id % 2 == 1) {
    throw std::invalid_argument("SeqId does not belong to a Node");
  }

  this->state_.state.lastNodeSequenceId = seq_id;
  this->state_.state.lastNodeId = this->getGraphIdBySeqIdAcquired(seq_id);

  auto match_ngt_seq = [seq_id](const auto &elem) { return elem.first <= seq_id; };

  erase_if(this->state_.node_state_by_seq, match_ngt_seq);
  erase_if(this->state_.edge_state_by_seq, match_ngt_seq);
}

void StateManager::setLastNode(const std::string &node_id) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.lastNodeId = node_id;
}

void StateManager::setAGVPosition(const vda5050pp::AGVPosition &position) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.agvPosition = position;
}

void StateManager::unsetAGVPosition() noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.agvPosition.reset();
}

void StateManager::setVelocity(const vda5050pp::Velocity &velocity) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.velocity = velocity;
}

void StateManager::unsetVelocity() noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.velocity.reset();
}

void StateManager::addLoad(const vda5050pp::Load &load) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.loads.push_back(load);
}

void StateManager::removeLoad(const std::string &load_id) noexcept(true) {
  auto lock = this->state_.acquire();

  auto match_load_id = [&load_id](const vda5050pp::Load &load) { return load.loadId == load_id; };

  auto it =
      std::remove_if(begin(this->state_.state.loads), end(this->state_.state.loads), match_load_id);

  this->state_.state.loads.erase(it);
}

void StateManager::removeLoad(const vda5050pp::Load &load) noexcept(true) {
  auto lock = this->state_.acquire();

  auto it = std::remove(begin(this->state_.state.loads), end(this->state_.state.loads), load);

  this->state_.state.loads.erase(it);
}

vda5050pp::Load StateManager::getLoad(const std::string &load_id) const noexcept(false) {
  auto lock = this->state_.acquireShared();

  auto match_load_id = [&load_id](const vda5050pp::Load &l) {
    return l.loadId.has_value() && *l.loadId == load_id;
  };

  auto it =
      std::find_if(cbegin(this->state_.state.loads), cend(this->state_.state.loads), match_load_id);

  if (it == cend(this->state_.state.loads)) {
    throw std::invalid_argument("No load with id: " + load_id);
  }

  return *it;
}

std::vector<vda5050pp::Load> StateManager::getLoads() const noexcept(true) {
  auto lock = this->state_.acquireShared();

  return this->state_.state.loads;
}

void StateManager::setDriving(bool driving) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.driving = driving;
}

bool StateManager::isDriving() const noexcept(true) { return this->state_.state.driving; }

void StateManager::requestNewBase() noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.newBaseRequested = true;
  // TODO: Notify
}

bool StateManager::isIdle() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  auto is_active = [](const auto &p) {
    return p.second.actionStatus != vda5050pp::ActionStatus::FINISHED &&
           p.second.actionStatus != vda5050pp::ActionStatus::FAILED;
  };
  auto action_running = std::any_of(cbegin(this->state_.action_state_by_id),
                                    cend(state_.action_state_by_id), is_active);

  return this->state_.node_state_by_seq.empty() && this->state_.edge_state_by_seq.empty() &&
         !action_running;
}

void StateManager::setDistanceSinceLastNode(double distance) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.distanceSinceLastNode = distance;
}

void StateManager::unsetDistanceSinceLastNode() noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.distanceSinceLastNode.reset();
}

vda5050pp::BatteryState StateManager::getBatteryState() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.batteryState;
}

void StateManager::setBatteryState(const vda5050pp::BatteryState &battery_state) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.batteryState = battery_state;
}

vda5050pp::SafetyState StateManager::getSafetyState() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.safetyState;
}

void StateManager::setSafetyState(const vda5050pp::SafetyState &safety_state) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.safetyState = safety_state;
}

void StateManager::addError(const vda5050pp::Error &error) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.errors.push_back(error);
}

size_t StateManager::removeError(
    const std::function<bool(const vda5050pp::Error &)> &pred) noexcept(true) {
  auto lock = this->state_.acquire();

  auto before_size = this->state_.state.errors.size();

  auto it = std::remove_if(begin(this->state_.state.errors), end(this->state_.state.errors), pred);

  if (it != this->state_.state.errors.end()) {
    this->state_.state.errors.erase(it);
  }

  return before_size - this->state_.state.errors.size();
}

bool StateManager::ensureError(const vda5050pp::Error &error) noexcept(true) {
  auto lock = this->state_.acquire();
  auto &errors = this->state_.state.errors;

  auto it = std::find(errors.begin(), errors.end(), error);
  if (it == errors.end()) {
    errors.push_back(error);
    return true;
  } else {
    return false;
  }
}

void StateManager::addInfo(const vda5050pp::Info &info) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.informations.push_back(info);
}

size_t StateManager::removeInfo(const std::function<bool(const vda5050pp::Info &)> &pred) noexcept(
    true) {
  auto lock = this->state_.acquire();

  auto before_size = this->state_.state.informations.size();

  auto it = std::remove_if(begin(this->state_.state.informations),
                           end(this->state_.state.informations), pred);

  if (it != this->state_.state.informations.end()) {
    this->state_.state.informations.erase(it);
  }

  return before_size - this->state_.state.informations.size();
}

uint32_t StateManager::nextStateSeq() noexcept(true) {
  auto lock = this->state_.acquire();
  return this->state_.state_seq_id++;
}

uint32_t StateManager::nextVisualizationSeq() noexcept(true) {
  auto lock = this->state_.acquire();
  return this->state_.visualization_seq_id++;
}

uint32_t StateManager::nextConnectionSeq() noexcept(true) {
  auto lock = this->state_.acquire();
  return this->state_.connection_seq_id++;
}

vda5050pp::State StateManager::dumpState() const noexcept(true) {
  auto lock = this->state_.acquireShared();

  vda5050pp::State state = this->state_.state;
  state.actionStates.clear();
  state.edgeStates.clear();
  state.nodeStates.clear();

  state.actionStates.reserve(state_.action_state_by_id.size());
  state.edgeStates.reserve(state_.edge_state_by_seq.size());
  state.nodeStates.reserve(state_.node_state_by_seq.size());

  auto snd = [](const auto &pair) { return pair.second; };
  std::transform(cbegin(this->state_.action_state_by_id), cend(this->state_.action_state_by_id),
                 std::back_inserter(state.actionStates), snd);
  std::transform(cbegin(this->state_.edge_state_by_seq), cend(this->state_.edge_state_by_seq),
                 std::back_inserter(state.edgeStates), snd);
  std::transform(cbegin(this->state_.node_state_by_seq), cend(this->state_.node_state_by_seq),
                 std::back_inserter(state.nodeStates), snd);

  return state;
}

bool StateManager::isNode(uint32_t seq) noexcept(true) { return seq % 2 == 0; }
bool StateManager::isEdge(uint32_t seq) noexcept(true) { return seq % 2 != 0; }

vda5050pp::Node StateManager::getNodeBySeq(uint32_t seq) const noexcept(false) {
  auto pair = this->state_.node_by_seq.find(seq);

  if (pair == end(this->state_.node_by_seq)) {
    throw std::invalid_argument("SequenceID does not belong to a Node");
  }

  return pair->second;
}

vda5050pp::Edge StateManager::getEdgeBySeq(uint32_t seq) const noexcept(false) {
  auto lock = this->state_.acquireShared();
  auto pair = this->state_.edge_by_seq.find(seq);

  if (pair == end(this->state_.edge_by_seq)) {
    throw std::invalid_argument("SequenceID does not belong to an Edge");
  }

  return pair->second;
}

uint32_t StateManager::getGraphBaseSeqId() const noexcept(true) {
  return this->state_.graph_base_seq_id;
}

uint32_t StateManager::getGraphHorizonSeqId() const noexcept(true) {
  auto lock = this->state_.acquireShared();

  auto cmp_fst = [](auto &a, auto &b) { return a.first < b.first; };

  return std::max_element(cbegin(this->state_.node_by_seq), cend(this->state_.node_by_seq), cmp_fst)
      ->first;
}

uint32_t StateManager::getAGVSequenceId() const noexcept(true) {
  return this->state_.state.lastNodeSequenceId;
}

const std::string &StateManager::getOrderId() const noexcept(true) {
  return this->state_.state.orderId;
}

uint32_t StateManager::getOrderUpdateId() const noexcept(true) {
  return this->state_.state.orderUpdateId;
}

bool StateManager::getPausedState() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.paused;
}

void StateManager::setPausedState(bool paused) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.paused = paused;
}

vda5050pp::OperatingMode StateManager::getOperatingMode() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.operatingMode;
}

void StateManager::setOperatingMode(const vda5050pp::OperatingMode &operating_mode) noexcept(true) {
  auto lock = this->state_.acquire();
  this->state_.state.operatingMode = operating_mode;
}

std::optional<vda5050pp::Node> StateManager::getNextNode() const noexcept(true) {
  auto lock = this->state_.acquireShared();

  auto next_node_seq = this->state_.state.lastNodeSequenceId + 2;

  auto it = this->state_.node_by_seq.find(next_node_seq);

  if (it != this->state_.node_by_seq.cend()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

std::list<vda5050pp::Node> StateManager::getBaseNodes() const noexcept(true) {
  std::list<vda5050pp::Node> ret;
  auto lock = this->state_.acquireShared();

  auto last_base_seq = this->state_.graph_base_seq_id;

  for (const auto &[seq, node] : this->state_.node_by_seq) {
    if (seq <= last_base_seq) {
      ret.push_back(node);
    }
  }

  return ret;
}

std::list<vda5050pp::Edge> StateManager::getBaseEdges() const noexcept(true) {
  std::list<vda5050pp::Edge> ret;
  auto lock = this->state_.acquireShared();

  auto last_base_seq = this->state_.graph_base_seq_id;

  for (const auto &[seq, edge] : this->state_.edge_by_seq) {
    if (seq <= last_base_seq) {
      ret.push_back(edge);
    }
  }

  return ret;
}

std::list<vda5050pp::Node> StateManager::getHorizonNodes() const noexcept(true) {
  std::list<vda5050pp::Node> ret;
  auto lock = this->state_.acquireShared();

  auto last_base_seq = this->state_.graph_base_seq_id;

  for (const auto &[seq, node] : this->state_.node_by_seq) {
    if (seq > last_base_seq) {
      ret.push_back(node);
    }
  }

  return ret;
}

std::list<vda5050pp::Edge> StateManager::getHorizonEdges() const noexcept(true) {
  std::list<vda5050pp::Edge> ret;
  auto lock = this->state_.acquireShared();

  auto last_base_seq = this->state_.graph_base_seq_id;

  for (const auto &[seq, edge] : this->state_.edge_by_seq) {
    if (seq > last_base_seq) {
      ret.push_back(edge);
    }
  }

  return ret;
}

std::optional<vda5050pp::AGVPosition> StateManager::getAGVPosition() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.agvPosition;
}

std::optional<vda5050pp::Velocity> StateManager::getVelocity() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.velocity;
}

std::string StateManager::getLastNodeId() const noexcept(true) {
  auto lock = this->state_.acquireShared();
  return this->state_.state.lastNodeId;
}

void StateManager::removeActionState(const std::string &actionId) noexcept(false) {
  auto lock = this->state_.acquire();

  this->state_.action_state_by_id.extract(actionId);
}