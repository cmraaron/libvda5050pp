// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/continuous_navigation_manager.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/net_manager.h"

using namespace vda5050pp::core::logic;

void ContinuousNavigationManager::startHandler() {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  this->handler_ = ha.createContinuousNavigationHandler();
  this->handler_->setHandle(this->handle_);
  this->handler_->setManager(*this);
  this->handler_->setInitialBase(std::move(this->start_nodes_), std::move(this->start_edges_));
  this->start_nodes_.clear();
  this->start_edges_.clear();

  q.push([this] {
    const auto &nodes = this->handler_->base_nodes_;
    const auto &edges = this->handler_->base_edges_;
    try {
      this->handler_->start(nodes, edges);
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "ContinuousNavigationManager::start()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logError(*error.errorDescription);
      }
      ha.getState().addError(error);
      ha.getLogic().abortOrder();
    }
  });
}

void ContinuousNavigationManager::destroyHandler() { this->handler_.reset(); }

void ContinuousNavigationManager::stepDrivingChanged(bool driving) const noexcept(true) {
  if (this->on_driving_changed_ != nullptr) {
    this->on_driving_changed_(driving);
  }
}

void ContinuousNavigationManager::stepEntered(ContinuousNavigationSingleStepTask &step) {
  this->current_step_ = step;

  if (this->first_node_seq_id_ == step.getNodeSequence()) {
    // The first step starts the handler
    this->startHandler();
  } else {
    // all other steps are always started
    step.started();
  }
}

void ContinuousNavigationManager::stepExited(ContinuousNavigationSingleStepTask &step) {
  this->current_step_.reset();

  if (this->last_node_seq_id_ == step.getNodeSequence()) {
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    ha.getState().setDriving(false);
    this->stepDrivingChanged(false);
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_medium);
    if (this->isFinalized()) {
      ha.getTaskQueue().push(std::bind(&ContinuousNavigationManager::destroyHandler, this));
    }
  }
}

void ContinuousNavigationManager::handlerFailed() {
  if (this->current_step_.has_value() && this->current_step_->get().failed()) {
    // OK
  } else {
    throw std::logic_error("ContinuousNavigation is not in a failable state");
  }
}

void ContinuousNavigationManager::handlerReachedNode(uint32_t node_seq) {
  if (this->current_step_.has_value() && this->current_step_->get().getNodeSequence() == node_seq &&
      this->current_step_->get().finished()) {
    // OK
  } else {
    throw std::logic_error("ContinuousNavigation cannot reach the given node");
  }
}

void ContinuousNavigationManager::handlerStarted() {
  if (this->current_step_.has_value() && this->current_step_->get().started()) {
    // OK
  } else {
    throw std::logic_error("ContinuousNavigation is not in a startable state");
  }
}

void ContinuousNavigationManager::handlerPaused() {
  if (this->current_step_.has_value() && this->current_step_->get().paused()) {
    // OK
  } else {
    throw std::logic_error("ContinuousNavigation is not in a pausable state");
  }
}

void ContinuousNavigationManager::handlerResumed() {
  if (this->current_step_.has_value() && this->current_step_->get().resumed()) {
    // OK
  } else {
    throw std::logic_error("ContinuousNavigation is not in a resumable state");
  }
}

void ContinuousNavigationManager::setOnDrivingChanged(const std::function<void(bool)> &fn) noexcept(
    true) {
  this->on_driving_changed_ = fn;
}

ContinuousNavigationManager::ContinuousNavigationManager(
    vda5050pp::interface_agv::Handle &handle, NetManager &net_manager, const vda5050pp::Node &node,
    std::vector<std::string> &&cancel_action_ids) noexcept(true)
    : first_node_seq_id_(node.sequenceId), handle_(handle), net_manager_(net_manager) {
  this->append(node, std::nullopt, std::move(cancel_action_ids));
}

ContinuousNavigationManager::ContinuousNavigationManager(
    vda5050pp::interface_agv::Handle &handle, NetManager &net_manager, const vda5050pp::Node &node,
    const vda5050pp::Edge &edge, std::vector<std::string> &&cancel_action_ids) noexcept(true)
    : first_node_seq_id_(node.sequenceId), handle_(handle), net_manager_(net_manager) {
  this->append(node, edge, std::move(cancel_action_ids));
}

void ContinuousNavigationManager::append(
    const vda5050pp::Node &node, const std::optional<vda5050pp::Edge> &edge,
    std::vector<std::string> &&cancel_action_ids) noexcept(true) {
  auto step = std::make_shared<ContinuousNavigationSingleStepTask>(handle_, *this, node.sequenceId,
                                                                   this->net_manager_.nextSeq());
  this->net_manager_.registerNewTaskManager(*step, std::move(cancel_action_ids));
  this->single_step_tasks_[node.sequenceId] = std::move(step);

  // Store for when the handler will be started (or appendings are commited)
  this->start_nodes_.push_back(node);
  if (edge.has_value()) {
    this->start_edges_.push_back(*edge);
  }

  this->last_node_seq_id_ = node.sequenceId;
}

void ContinuousNavigationManager::commitAppendings() {
  // If handler already started
  if (this->handler_ != nullptr && !this->finalized_) {
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    auto &q = ha.getTaskQueue();
    this->handler_->appendToBase(std::move(this->start_nodes_), std::move(this->start_edges_));
    this->start_nodes_.clear();
    this->start_edges_.clear();
    q.push([this] {
      auto nodes = this->handler_->getBaseDeltaNodes();
      auto edges = this->handler_->getBaseDeltaEdges();
      try {
        this->handler_->baseIncreased(nodes, edges);
      } catch (const std::exception &e) {
        vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
        auto error = vda5050pp::core::common::errorFromInternalException(
            e, "ContinuousNavigationManager::baseIncreased()");
        if (error.errorDescription.has_value()) {
          ha.getLogger().logError(*error.errorDescription);
        }
        ha.getState().addError(error);
        ha.getLogic().abortOrder();
      }
    });
  }
}

void ContinuousNavigationManager::finalize() {
  this->commitAppendings();
  this->finalized_ = true;
}
bool ContinuousNavigationManager::isFinalized() const { return this->finalized_; }

void ContinuousNavigationManager::pause() {
  if (this->handler_ != nullptr) {
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    auto &q = ha.getTaskQueue();
    q.push([this] {
      try {
        this->handler_->pause();
      } catch (const std::exception &e) {
        vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
        auto error = vda5050pp::core::common::errorFromInternalException(
            e, "ContinuousNavigationManager::pause()");
        if (error.errorDescription.has_value()) {
          ha.getLogger().logError(*error.errorDescription);
        }
        ha.getState().addError(error);
        ha.getLogic().abortOrder();
      }
    });
  }
}

void ContinuousNavigationManager::resume() {
  if (this->handler_ != nullptr) {
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    auto &q = ha.getTaskQueue();
    q.push([this] {
      try {
        this->handler_->resume();
      } catch (const std::exception &e) {
        vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
        auto error = vda5050pp::core::common::errorFromInternalException(
            e, "ContinuousNavigationManager::resume()");
        if (error.errorDescription.has_value()) {
          ha.getLogger().logError(*error.errorDescription);
        }
        ha.getState().addError(error);
        ha.getLogic().abortOrder();
      }
    });
  }
}

void ContinuousNavigationManager::stop() {
  if (this->handler_ != nullptr) {
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    auto &q = ha.getTaskQueue();
    q.push([this] {
      try {
        this->handler_->stop();
      } catch (const std::exception &e) {
        vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
        auto error = vda5050pp::core::common::errorFromInternalException(
            e, "ContinuousNavigationManager::stop()");
        if (error.errorDescription.has_value()) {
          ha.getLogger().logError(*error.errorDescription);
        }
        ha.getState().addError(error);
        ha.getLogic().abortOrder();
      }
    });
  }
}

void ContinuousNavigationManager::horizonChanged() {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &q = ha.getTaskQueue();
  auto &state = ha.getState();

  if (this->handler_ == nullptr) {
    return;
  }

  this->handler_->setNewHorizon(state.getHorizonNodes(), state.getHorizonEdges());

  q.push([this] {
    auto &nodes = this->handler_->getHorizonNodes();
    auto &edges = this->handler_->getHorizonEdges();
    try {
      this->handler_->horizonUpdated(nodes, edges);
    } catch (const std::exception &e) {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      auto error = vda5050pp::core::common::errorFromInternalException(
          e, "ContinuousNavigationManager::horizonChanged()");
      if (error.errorDescription.has_value()) {
        ha.getLogger().logWarn(*error.errorDescription);
      }
      error.errorLevel = vda5050pp::ErrorLevel::WARNING;
      ha.getState().addError(error);
    }
  });
}

bool ContinuousNavigationManager::intercept() noexcept(true) {
  return this->current_step_.has_value() && this->current_step_->get().intercept();
}

bool ContinuousNavigationManager::isActive() const noexcept(true) {
  return this->current_step_.has_value();
}

bool ContinuousNavigationManager::cancel() const noexcept(true) {
  bool success = true;
  for (auto &[_, step] : this->single_step_tasks_) {
    if (!(step->isActive() || step->isDone())) {
      success = success && step->cancel();
    }
  }
  return success;
}

std::shared_ptr<ContinuousNavigationSingleStepTask>
ContinuousNavigationManager::getInterceptedMgr() noexcept(true) {
  if (!this->current_step_.has_value()) {
    return nullptr;
  }

  auto seq = this->current_step_->get().getNodeSequence();
  return this->single_step_tasks_.at(seq);
}
