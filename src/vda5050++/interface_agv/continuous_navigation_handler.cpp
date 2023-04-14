// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/interface_agv/continuous_navigation_handler.h"

#include <algorithm>

#include "vda5050++/core/interface_agv/const_handle_accessor.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/continuous_navigation_manager.h"

void vda5050pp::interface_agv::ContinuousNavigationHandler::setHandle(
    vda5050pp::interface_agv::Handle &handle) noexcept(true) {
  this->handle_ = &handle;
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setManager(
    vda5050pp::core::logic::ContinuousNavigationManager &mgr) noexcept(true) {
  this->manager_ = &mgr;
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setInitialBase(
    std::list<vda5050pp::Node> &&initial_base_nodes,
    std::list<vda5050pp::Edge> &&initial_base_edges) {
  this->base_nodes_ = std::move(initial_base_nodes);
  this->base_edges_ = std::move(initial_base_edges);
  initial_base_nodes.clear();
  initial_base_edges.clear();
  this->base_old_last_edge_ =
      this->base_edges_.empty() ? this->base_edges_.end() : --this->base_edges_.end();
  this->base_old_last_node_ =
      this->base_nodes_.empty() ? this->base_nodes_.end() : --this->base_nodes_.end();
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setNewHorizon(
    std::list<vda5050pp::Node> &&new_horizon_nodes,
    std::list<vda5050pp::Edge> &&new_horizon_edges) {
  this->horz_nodes_ = std::move(new_horizon_nodes);
  this->horz_edges_ = std::move(new_horizon_edges);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::appendToBase(
    std::list<vda5050pp::Node> &&new_base_nodes, std::list<vda5050pp::Edge> &&new_base_edges) {
  this->base_old_last_edge_ =
      this->base_edges_.empty() ? this->base_edges_.end() : --this->base_edges_.end();
  this->base_old_last_node_ =
      this->base_nodes_.empty() ? this->base_nodes_.end() : --this->base_nodes_.end();

  this->base_nodes_.splice(this->base_nodes_.end(), std::move(new_base_nodes));
  this->base_edges_.splice(this->base_edges_.end(), std::move(new_base_edges));

  if (this->base_old_last_node_ == this->base_nodes_.end()) {
    this->base_old_last_node_ = this->base_nodes_.begin();
  } else {
    this->base_old_last_node_++;
  }
  if (this->base_old_last_edge_ == this->base_edges_.end()) {
    this->base_old_last_edge_ = this->base_edges_.begin();
  } else {
    this->base_old_last_edge_++;
  }
}

const vda5050pp::Node &vda5050pp::interface_agv::ContinuousNavigationHandler::getLastNode() const {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  auto &state = ha.getState();

  auto seq = state.getAGVSequenceId();
  auto match_seq = [&seq](const vda5050pp::Node &n) { return n.sequenceId == seq; };

  // This has to be found
  return *std::find_if(this->base_nodes_.cbegin(), this->base_nodes_.cend(), match_seq);
}

std::optional<std::reference_wrapper<const vda5050pp::Node>>
vda5050pp::interface_agv::ContinuousNavigationHandler::getNextNode() const {
  if (this->base_nodes_.size() >= 1) {
    return std::ref(this->base_nodes_.front());
  } else {
    return std::nullopt;
  }
}

std::optional<std::reference_wrapper<const vda5050pp::Edge>>
vda5050pp::interface_agv::ContinuousNavigationHandler::getNextEdge() const {
  if (this->base_edges_.size() < 1) {
    return std::nullopt;
  } else {
    return std::ref(this->base_edges_.front());
  }
}

const std::list<vda5050pp::Node>
    &vda5050pp::interface_agv::ContinuousNavigationHandler::getBaseNodes() const {
  return this->base_nodes_;
}
const std::list<vda5050pp::Edge>
    &vda5050pp::interface_agv::ContinuousNavigationHandler::getBaseEdges() const {
  return this->base_edges_;
}
const std::list<vda5050pp::Node>
    &vda5050pp::interface_agv::ContinuousNavigationHandler::getHorizonNodes() const {
  return this->horz_nodes_;
}
const std::list<vda5050pp::Edge>
    &vda5050pp::interface_agv::ContinuousNavigationHandler::getHorizonEdges() const {
  return this->horz_edges_;
}
std::list<vda5050pp::Node>
vda5050pp::interface_agv::ContinuousNavigationHandler::getBaseDeltaNodes() const {
  std::list<vda5050pp::Node> ret;
  std::list<vda5050pp::Node>::const_iterator beg = this->base_old_last_node_;
  if (beg != this->base_nodes_.cend()) {
    std::copy(beg, this->base_nodes_.cend(), std::back_inserter(ret));
  }

  return ret;
}

std::list<vda5050pp::Edge>
vda5050pp::interface_agv::ContinuousNavigationHandler::getBaseDeltaEdges() const {
  std::list<vda5050pp::Edge> ret;
  std::list<vda5050pp::Edge>::const_iterator beg = this->base_old_last_edge_;
  if (beg != this->base_edges_.cend()) {
    std::copy(beg, this->base_edges_.cend(), std::back_inserter(ret));
  }

  return ret;
}

bool vda5050pp::interface_agv::ContinuousNavigationHandler::evalPosition(
    const vda5050pp::AGVPosition &position) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  bool reached = false;
  auto next_node = this->getNextNode();

  if (next_node.has_value() && next_node->get().nodePosition.has_value() &&
      next_node->get().nodePosition->mapId == position.mapId) {
    using namespace vda5050pp::core::common;
    auto &node = next_node.value().get();

    // only match if AGV and node are on the same map
    math::Circle<double> node_deviation;
    math::Circle<double> agv_deviation;
    node_deviation.origin = {node.nodePosition->x, node.nodePosition->y};
    node_deviation.radius = node.nodePosition->allowedDeviationXY.value_or(0);
    agv_deviation.origin = {position.x, position.y};
    agv_deviation.radius = position.deviationRange.value_or(0);

    // Check if theta matches
    auto theta_match = true;
    if (node.nodePosition->allowedDeviationTheta.has_value() &&
        node.nodePosition->theta.has_value()) {
      auto theta_diff = math::angleDifference(position.theta, *node.nodePosition->theta);
      theta_match = theta_diff <= *node.nodePosition->allowedDeviationTheta;
    }

    // If position and theta matches, set node to reached
    if (math::circleEnclosureOf(node_deviation, agv_deviation) && theta_match) {
      this->setNodeReached(node.sequenceId);
      reached = true;
    }
  }

  this->setPosition(position);
  return reached;
}

bool vda5050pp::interface_agv::ContinuousNavigationHandler::evalPosition(
    double x, double y, double theta, const std::string &map) noexcept(false) {
  vda5050pp::AGVPosition position;
  position.x = x;
  position.y = y;
  position.theta = theta;
  position.mapId = map;
  position.positionInitialized = true;
  position.deviationRange = std::nullopt;
  position.localizationScore = std::nullopt;
  return this->evalPosition(position);
}

bool vda5050pp::interface_agv::ContinuousNavigationHandler::evalPosition(
    double x, double y, double theta, const std::string &map, double deviation) noexcept(false) {
  vda5050pp::AGVPosition position;
  position.x = x;
  position.y = y;
  position.theta = theta;
  position.mapId = map;
  position.positionInitialized = true;
  position.deviationRange = deviation;
  position.localizationScore = std::nullopt;
  return this->evalPosition(position);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setPosition(
    const vda5050pp::AGVPosition &position) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);

  this->path_length_calculator_.addVertex(
      {position.x, position.y});  // assumes the map is not changed (as in vda5050)

  ha.getState().setDistanceSinceLastNode(this->path_length_calculator_.getLength());
  ha.getState().setAGVPosition(position);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setPosition(
    double x, double y, double theta, const std::string &map) noexcept(false) {
  vda5050pp::AGVPosition position;
  position.x = x;
  position.y = y;
  position.theta = theta;
  position.mapId = map;
  position.positionInitialized = true;
  position.deviationRange = std::nullopt;
  position.localizationScore = std::nullopt;
  this->setPosition(position);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setPosition(
    double x, double y, double theta, const std::string &map, double deviation) noexcept(false) {
  vda5050pp::AGVPosition position;
  position.x = x;
  position.y = y;
  position.theta = theta;
  position.mapId = map;
  position.deviationRange = deviation;
  position.positionInitialized = true;
  position.localizationScore = std::nullopt;
  this->setPosition(position);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::addError(const std::string &description,
                                                                     vda5050pp::ErrorLevel level) {
  this->addError(description, level, {});
}
void vda5050pp::interface_agv::ContinuousNavigationHandler::addError(
    const std::string &description, vda5050pp::ErrorLevel level,
    const std::vector<vda5050pp::ErrorReference> &additional_references) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  auto &state = ha.getState();

  auto node = state.getNextNode().value_or(this->getLastNode());

  vda5050pp::Error error;
  error.errorDescription = description;
  error.errorLevel = level;
  error.errorType = "NavigationError";
  error.errorReferences = {{"node.sequenceId", std::to_string(node.sequenceId)}};

  error.errorReferences->reserve(additional_references.size() + error.errorReferences->size());
  std::move(additional_references.begin(), additional_references.end(),
            std::back_inserter(*error.errorReferences));

  state.addError(error);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
}
void vda5050pp::interface_agv::ContinuousNavigationHandler::addInfo(const std::string &description,
                                                                    vda5050pp::InfoLevel level) {
  this->addInfo(description, level, {});
}
void vda5050pp::interface_agv::ContinuousNavigationHandler::addInfo(
    const std::string &description, vda5050pp::InfoLevel level,
    const std::vector<vda5050pp::InfoReference> &additional_references) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);
  auto &state = ha.getState();

  auto node = state.getNextNode().value_or(this->getLastNode());

  vda5050pp::Info info;
  info.infoDescription = description;
  info.infoLevel = level;
  info.infoType = "NavigationInfo";
  info.infoReferences = {{"node.sequenceId", std::to_string(node.sequenceId)}};

  info.infoReferences->reserve(additional_references.size() + info.infoReferences->size());
  std::move(additional_references.begin(), additional_references.end(),
            std::back_inserter(*info.infoReferences));

  state.addInfo(info);
  ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::failed() {
  this->manager_->handlerFailed();
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::started() {
  this->manager_->handlerStarted();
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::paused() {
  this->manager_->handlerPaused();
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::resumed() {
  this->manager_->handlerResumed();
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setNodeReached(
    const vda5050pp::Node &node) noexcept(false) {
  this->setNodeReached(node.sequenceId);
}

void vda5050pp::interface_agv::ContinuousNavigationHandler::setNodeReached(
    uint32_t node_seq) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(*this->handle_);

  this->manager_->handlerReachedNode(node_seq);

  auto seq_leq = [node_seq](const auto &elem) { return elem.sequenceId <= node_seq; };

  this->base_edges_.remove_if(seq_leq);
  this->base_nodes_.remove_if(seq_leq);
  this->base_old_last_edge_ =
      this->base_edges_.end();  // no reset needed, TODO: only change if removed
  this->base_old_last_node_ = this->base_nodes_.end();
}