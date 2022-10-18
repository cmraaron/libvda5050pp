// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/net_manager.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <utility>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/core/logic/cancel_net.h"
#include "vda5050++/core/logic/dangling_net.h"
#include "vda5050++/core/logic/drive_to_node_manager.h"
#include "vda5050++/core/logic/id_not_found_error.h"
#include "vda5050++/core/logic/parallel_launch_net.h"
#include "vda5050++/core/logic/sync_net.h"

using namespace vda5050pp::core::logic;

NetManager::NetManager(vda5050pp::interface_agv::Handle &handle) noexcept(true) : handle_(handle) {
  this->tail_place_ = this->net_.addPlace({LogicTaskNetTypes::k_done, this->next_seq_++}, 1);
}

void NetManager::insertTimeStepAction(const std::vector<std::string> &action_ids,
                                      bool force_sync) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState();

  if (action_ids.empty()) return;

  std::vector<LogicTaskNetID> ready_ids;
  std::vector<LogicTaskNetID> done_ids;
  std::vector<LogicTaskNetID> finish_ids;
  std::vector<LogicTaskNetID> fail_ids;

  ha.getLogger().logDebug(vda5050pp::core::common::logstring("---BEGIN ACTION GROUP---"));

  // Create manager for each action
  for (const auto &id : action_ids) {
    auto action = state.getActionById(id);
    bool sync_this_action = action.blockingType != vda5050pp::BlockingType::NONE || force_sync;

    ha.getLogger().logDebug(vda5050pp::core::common::logstring(
        "Inserting Action ", id, "@", this->next_seq_, " sync: ", sync_this_action));

    auto mgr_seq = this->next_seq_++;
    auto mgr_ptr = std::make_shared<ActionManager>(this->handle_, std::move(action), mgr_seq);

    ready_ids.push_back(mgr_ptr->readyPlace());
    finish_ids.push_back(mgr_ptr->finishedPlace());
    fail_ids.push_back(mgr_ptr->failedPlace());
    if (sync_this_action) {
      done_ids.push_back(mgr_ptr->donePlace());
    }

    mgr_ptr->attachToNet(this->net_);

    // Exit logic. Call on_all_exited when each mgr reached it's exited place
    this->un_exited_ids_.insert(mgr_ptr->exitedPlace());
    mgr_ptr->onExited([this](const auto &id) {
      this->un_exited_ids_.extract(id);
      vda5050pp::core::interface_agv::HandleAccessor(this->handle_)
          .getLogger()
          .logDebug(vda5050pp::core::common::format("Mgr exited: #UnExited={}",
                                                    this->un_exited_ids_.size()));
      if (this->un_exited_ids_.empty() && this->on_all_exited_ != nullptr) {
        this->on_all_exited_();
      }
    });

    this->action_managers_by_id_[id] = std::move(mgr_ptr);
  }

  auto id_to_place_ptr = [this](const LogicTaskNetID &id) -> std::shared_ptr<Net::PlaceT> {
    auto ptr = this->net_.findPlace(id);
    if (ptr == nullptr) {
      throw IdNotFoundError(id);
    }
    return ptr;
  };

  // register finish and fail places
  std::transform(begin(finish_ids), end(finish_ids), std::back_inserter(this->finish_places_),
                 id_to_place_ptr);
  std::transform(begin(fail_ids), end(fail_ids), std::back_inserter(this->fail_places_),
                 id_to_place_ptr);

  std::shared_ptr<Net::PlaceT> new_tail_place;

  // When there is nothing to synchronize
  // we still have to make sure things are launched in parallel once.
  // The ParallelLaunchNet will directly launch a SyncNet with no
  // SyncPlaces.
  if (done_ids.empty()) {
    // Empty sync case
    LogicTaskNetID sync_id = {LogicTaskNetTypes::k_combinator_sync, this->next_seq_};
    ready_ids.push_back(sync_id);
    new_tail_place = this->net_.addPlace(sync_id, 0);

    ParallelLaunchNet pl(this->tail_place_->getID(), std::move(ready_ids));
    pl.attachToNet(this->net_);

  } else {
    // Normal case
    ParallelLaunchNet pl(this->tail_place_->getID(), std::move(ready_ids));
    pl.attachToNet(this->net_);

    SyncNet sn(std::move(done_ids), {LogicTaskNetTypes::k_combinator_sync, this->next_seq_++});
    sn.attachToNet(this->net_);

    new_tail_place = this->net_.findPlace(sn.getPlaceID());
    if (new_tail_place == nullptr) {
      throw IdNotFoundError(sn.getPlaceID());
    }
  }

  this->tail_place_->onChange(nullptr);
  this->tail_place_ = new_tail_place;
  this->tail_place_->onChange([this](auto, auto) {
    if (this->on_tail_reached_ != nullptr) this->on_tail_reached_();
  });

  ha.getLogger().logDebug(vda5050pp::core::common::logstring("---END ACTION GROUP---"));
}

void NetManager::insertTimeStepDriveToNode(
    const vda5050pp::Node &node, const vda5050pp::Edge &edge,
    std::vector<std::string> &&cancel_action_ids) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getLogger().logDebug(
      vda5050pp::core::common::logstring("Inserting DTN Timestap ", this->next_seq_));

  if (ha.isContinuousNavigation()) {
    if (this->continuous_navigation_managers_.empty() ||
        this->continuous_navigation_managers_.front()->isFinalized()) {
      // No old manager available, or it is finalized -> create a new one
      ha.getLogger().logDebug(
          vda5050pp::core::common::logstring("Creating new CNavManager ", this->next_seq_));

      this->continuous_navigation_managers_.push_front(
          std::make_shared<ContinuousNavigationManager>(this->handle_, *this, node, edge,
                                                        std::move(cancel_action_ids)));
      this->continuous_navigation_managers_.front()->setOnDrivingChanged(this->on_driving_changed_);
    } else {
      ha.getLogger().logDebug(vda5050pp::core::common::logstring("Appending to CNavManager"));
      this->continuous_navigation_managers_.front()->append(node, edge,
                                                            std::move(cancel_action_ids));
    }

    return;
  }

  // Create manager
  auto mgr_seq = this->next_seq_++;
  auto mgr_ptr = std::make_shared<DriveToNodeManager>(ha.getHandle(), edge, node, mgr_seq);
  mgr_ptr->onDrivingChanged(this->on_driving_changed_);

  mgr_ptr->attachToNet(this->net_);

  // Exit logic. Call on all exited when each mgr reached it's exited place
  this->un_exited_ids_.insert(mgr_ptr->exitedPlace());
  mgr_ptr->onExited([this, ha](const auto &id) {
    this->un_exited_ids_.extract(id);
    if (this->un_exited_ids_.empty() && this->on_all_exited_ != nullptr) {
      using namespace std::string_literals;
      ha.getLogger().logDebug(vda5050pp::core::common::logstring("Last exited: ", id));
      this->on_all_exited_();
    }
  });

  auto id_to_place_ptr = [this](const LogicTaskNetID &id) -> std::shared_ptr<Net::PlaceT> {
    auto ptr = this->net_.findPlace(id);
    if (ptr == nullptr) {
      throw IdNotFoundError(id);
    }
    return ptr;
  };

  // Save fail and finish places
  this->fail_places_.push_back(id_to_place_ptr(mgr_ptr->failedPlace()));
  this->finish_places_.push_back(id_to_place_ptr(mgr_ptr->finishedPlace()));

  // Create launch net
  ParallelLaunchNet pl(this->tail_place_->getID(), {mgr_ptr->readyPlace()});
  pl.attachToNet(this->net_);

  // Create cancel net
  CancelNet cn(std::move(cancel_action_ids), mgr_ptr->donePlace(), *this);
  cn.attachToNet(this->net_);

  // Set new tail place
  this->tail_place_->onChange(nullptr);
  this->tail_place_ = id_to_place_ptr(cn.getPlaceID());
  this->tail_place_->onChange([this](auto, auto) {
    if (this->on_tail_reached_ != nullptr) this->on_tail_reached_();
  });

  this->drive_to_node_managers_by_id_[node.sequenceId] = std::move(mgr_ptr);
}

void NetManager::interpretNode(const vda5050pp::Node &node) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  std::vector<std::string> parallel_action_ids;

  ha.getLogger().logDebug(
      vda5050pp::core::common::logstring("Interpreting Node ", node.nodeId, "@", node.sequenceId));

  // See vda5050 1.1 - 6.12
  for (const auto &action : node.actions) {
    switch (action.blockingType) {
      case vda5050pp::BlockingType::HARD:
        if (ha.isContinuousNavigation() && !this->continuous_navigation_managers_.empty()) {
          this->continuous_navigation_managers_.front()->finalize();
        }
        this->insertTimeStepAction(parallel_action_ids, true);
        parallel_action_ids.clear();
        this->insertTimeStepAction({action.actionId}, false);
        break;

      case vda5050pp::BlockingType::SOFT:
        if (ha.isContinuousNavigation() && !this->continuous_navigation_managers_.empty()) {
          this->continuous_navigation_managers_.front()->finalize();
        }
        parallel_action_ids.push_back(action.actionId);
        break;
      case vda5050pp::BlockingType::NONE:
        parallel_action_ids.push_back(action.actionId);
        break;
      default:
        ha.getLogger().logFatal(LOGSTRING(vda5050pp::core::common::format(
            "BUG: unknown blocking type {}", (int)action.blockingType)));
        break;
    }
  }

  if (!parallel_action_ids.empty()) {
    this->insertTimeStepAction(parallel_action_ids, false);
  }
}

void NetManager::interpretEdgeThenNode(const vda5050pp::Edge &edge,
                                       const vda5050pp::Node &node) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  std::vector<std::string> parallel_action_ids;

  ha.getLogger().logDebug(
      vda5050pp::core::common::logstring("Interpreting Edge ", edge.edgeId, "@", edge.sequenceId));

  // See vda5050 1.1 - 6.12
  for (const auto &action : edge.actions) {
    switch (action.blockingType) {
      case vda5050pp::BlockingType::HARD:
        if (ha.isContinuousNavigation()) {
          this->continuous_navigation_managers_.front()->finalize();
        }
        this->insertTimeStepAction(parallel_action_ids, true);
        parallel_action_ids.clear();
        this->insertTimeStepAction({action.actionId}, false);
        break;

      case vda5050pp::BlockingType::SOFT:
        if (ha.isContinuousNavigation()) {
          this->continuous_navigation_managers_.front()->finalize();
        }
        parallel_action_ids.push_back(action.actionId);
        break;

      case vda5050pp::BlockingType::NONE:
        parallel_action_ids.push_back(action.actionId);
        break;
      default:
        ha.getLogger().logFatal(LOGSTRING(vda5050pp::core::common::format(
            "BUG: unknown blocking type {}", (int)action.blockingType)));
        break;
    }
  }

  if (!parallel_action_ids.empty()) {
    this->insertTimeStepAction(parallel_action_ids, false);
  }

  insertTimeStepDriveToNode(node, edge, std::move(parallel_action_ids));

  interpretNode(node);
}

void NetManager::interpret() noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  auto &state = ha.getState().getStateUnsafe();
  auto &state_mgr = ha.getState();

  // If only one Node must be interpreted, it can be done on it's own.
  // Otherwise if an uninterpreted Edge is before that Node, actions
  // from that Edge have to be canceled -> interpret them together
  if (state.graph_base_seq_id >= state.graph_next_interpreted_seq_id_) {
    ha.getLogger().logDebug("interpret() next step");
    if (vda5050pp::core::state::StateManager::isNode(state.graph_next_interpreted_seq_id_)) {
      auto interpret_seq = state.graph_next_interpreted_seq_id_++;
      this->interpretNode(state_mgr.getNodeBySeq(interpret_seq));
    } else {
      auto eid = state.graph_next_interpreted_seq_id_++;
      auto nid = state.graph_next_interpreted_seq_id_++;
      this->interpretEdgeThenNode(state_mgr.getEdgeBySeq(eid), state_mgr.getNodeBySeq(nid));
    }

    this->interpret();
  } else {
    ha.getLogger().logDebug("interpret() last step");
    if (!this->continuous_navigation_managers_.empty()) {
      ha.getLogger().logDebug("interpret() commit");
      this->continuous_navigation_managers_.front()->commitAppendings();
      this->notifyHorizonChanged();
    }
    if (!state_mgr.getPausedState()) {
      // Finally tick to re-/start if not paused
      this->tick();
    }
  }
}

void NetManager::clear() noexcept(true) {
  this->action_managers_by_id_.clear();
  this->drive_to_node_managers_by_id_.clear();
  this->fail_places_.clear();
  this->finish_places_.clear();
  this->on_all_exited_ = nullptr;
  this->on_driving_changed_ = nullptr;
  this->on_tail_reached_ = nullptr;
  this->un_exited_ids_.clear();
  this->net_ = Net();
  this->next_seq_ = 1;
  this->tail_place_ = this->net_.addPlace({LogicTaskNetTypes::k_done, 0}, 1);
  this->continuous_navigation_managers_.clear();
}

void NetManager::tick() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getLogger().logDebug("NetManager::tick()");
  this->net_.deepTickCover();
}

void NetManager::pauseAction(const std::string &action_id) noexcept(false) {
  auto pair = this->action_managers_by_id_.find(action_id);

  if (pair == end(this->action_managers_by_id_)) {
    throw std::invalid_argument("No ActionManager for action_id: " + action_id);
  }

  pair->second->pause();
}

void NetManager::resumeAction(const std::string &action_id) noexcept(false) {
  auto pair = this->action_managers_by_id_.find(action_id);

  if (pair == end(this->action_managers_by_id_)) {
    throw std::invalid_argument("No ActionManager for action_id: " + action_id);
  }

  pair->second->resume();
}

void NetManager::stopAction(const std::string &action_id) noexcept(false) {
  auto pair = this->action_managers_by_id_.find(action_id);

  if (pair == end(this->action_managers_by_id_)) {
    throw std::invalid_argument("No ActionManager for action_id: " + action_id);
  }

  pair->second->stop();
}

bool NetManager::isActionActive(const std::string &action_id) noexcept(false) {
  auto pair = this->action_managers_by_id_.find(action_id);

  if (pair == end(this->action_managers_by_id_)) {
    throw std::invalid_argument("No ActionManager for action_id: " + action_id);
  }

  return pair->second->isActive();
}

void NetManager::pauseAllRunningActions() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isRunning()) {
      mgr->pause();
    }
  }
}

void NetManager::resumeAllPausedActions() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isPaused()) {
      mgr->resume();
    }
  }
}

void NetManager::stopAllActive() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isActive()) {
      mgr->stop();
    }
  }
  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    if (mgr->isActive()) {
      mgr->stop();
    }
  }
  for (auto &mgr : this->continuous_navigation_managers_) {
    if (mgr->isActive()) {
      mgr->stop();
    }
  }
}

void NetManager::cancelAll() noexcept(true) {
  this->cancelAllPending();
  this->stopAllActive();
}

void NetManager::cancelAllPending() noexcept(true) {
  // Cancel all tasks in the future
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (!mgr->isDone() && !mgr->isActive()) {
      mgr->cancel();
    }
  }
  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    if (!mgr->isDone() && !mgr->isActive()) {
      mgr->cancel();
    }
  }
  for (auto &mgr : this->continuous_navigation_managers_) {
    mgr->cancel();
  }
}

std::vector<std::shared_ptr<ActionManager>> NetManager::interceptAllActive() noexcept(true) {
  std::vector<std::shared_ptr<ActionManager>> ret;
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isActive()) {
      if (mgr->intercept()) {
        ret.push_back(mgr);
      }
    }
  }
  return ret;
}

std::shared_ptr<TaskManager> NetManager::interceptDriving() noexcept(true) {
  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    if (mgr->isActive()) {
      if (mgr->intercept()) {
        return mgr;
      }
    }
  }
  for (auto &mgr : this->continuous_navigation_managers_) {
    if (mgr->isActive()) {
      if (mgr->intercept()) {
        return mgr->getInterceptedMgr();
      }
    }
  }
  return nullptr;
}

void NetManager::stopHard() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isActive() &&
        mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::HARD) {
      mgr->stop();
    }
  }
}

void NetManager::stopSoft() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isActive() &&
        mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::SOFT) {
      mgr->stop();
    }
  }
}

void NetManager::stopNone() noexcept(true) {
  for (auto &[_, mgr] : this->action_managers_by_id_) {
    if (mgr->isActive() &&
        mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::NONE) {
      mgr->stop();
    }
  }
}

void NetManager::interceptWithAction(const vda5050pp::Action &action) noexcept(true) {
  auto mgr = std::make_shared<ActionManager>(this->handle_, action, this->next_seq_++);

  auto mgr_begin_id = [](const std::shared_ptr<ActionManager> &mgr) -> LogicTaskNetID {
    return mgr->interceptingBeginPlace();
  };
  auto mgr_end_id = [](const std::shared_ptr<ActionManager> &mgr) -> LogicTaskNetID {
    return mgr->interceptingEndPlace();
  };

  // Get ids && mgrs
  std::vector<LogicTaskNetID> begin_ids;
  std::vector<LogicTaskNetID> end_ids;
  auto intercepted_mgrs = this->interceptAllActive();
  std::transform(cbegin(intercepted_mgrs), cend(intercepted_mgrs), std::back_inserter(begin_ids),
                 mgr_begin_id);
  std::transform(cbegin(intercepted_mgrs), cend(intercepted_mgrs), std::back_inserter(end_ids),
                 mgr_end_id);

  if (begin_ids.empty()) {
    // There is no action running.
    // The intercepting mgr cannot be attached to a running task,
    // therefor a dangling task must be created.

    auto intercepted = this->interceptDriving();

    DanglingNet dn(mgr->readyPlace());
    mgr->attachToNet(this->net_);
    dn.attachToNet(this->net_);

    if (intercepted != nullptr) {
      SyncNet sn({mgr->donePlace(), intercepted->interceptingBeginPlace()},
                 intercepted->interceptingEndPlace());
      sn.attachToNet(this->net_);

      if (action.blockingType != vda5050pp::BlockingType::NONE) {
        this->pauseDriving();
        mgr->onExited([this](auto) { this->resumeDriving(); });
      }
      intercepted->interceptSequential();
    } else {
      // No action or driving manager is active
      // Attach to the tail of the current net
      SyncNet sn({mgr->donePlace(), this->tail_place_->getID()},
                 {LogicTaskNetTypes::k_combinator_sync, this->next_seq_++});
      sn.attachToNet(this->net_);
      this->tail_place_ = this->net_.findPlace(sn.getPlaceID());
    }

    this->action_managers_by_id_[action.actionId] = mgr;
    this->net_.findTransition(dn.getTransition())->deepFire();
    return;
  }

  // Find current blocking type
  auto blocking_type_ceiling = [](vda5050pp::BlockingType current,
                                  const std::shared_ptr<ActionManager> &mgr) {
    switch (current) {
      case vda5050pp::BlockingType::HARD:
        return current;
      case vda5050pp::BlockingType::SOFT:
        if (mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::HARD) {
          return vda5050pp::BlockingType::HARD;
        } else {
          return current;
        }
      default:
      case vda5050pp::BlockingType::NONE:
        if (mgr->getHandler()->getAction().blockingType != vda5050pp::BlockingType::NONE) {
          return mgr->getHandler()->getAction().blockingType;
        } else {
          return current;
        }
    }
  };
  auto current_blocking_type =
      std::accumulate(cbegin(intercepted_mgrs), cend(intercepted_mgrs),
                      vda5050pp::BlockingType::NONE, blocking_type_ceiling);

  // Do interrupt
  if (action.blockingType == vda5050pp::BlockingType::HARD) {
    // The AGV has to cancel all running Actions and stop driving, nothing can run in parallel
    SyncNet sn(std::move(begin_ids), mgr->readyPlace());
    ParallelLaunchNet pn(mgr->donePlace(), std::move(end_ids));
    mgr->attachToNet(this->net_);
    if (current_blocking_type == vda5050pp::BlockingType::NONE) {
      // The AGV might be driving
      this->pauseDriving();
      mgr->onExited([this](auto) { this->resumeDriving(); });
    }
    sn.attachToNet(this->net_);
    pn.attachToNet(this->net_);
    // stop all running actions
    this->stopHard();
    this->stopSoft();
    this->stopNone();
    // the interrupting action can only run once the preceeding tasks are done
    for (auto mgr : intercepted_mgrs) {
      mgr->interceptSequential();
    }
  } else if (action.blockingType == vda5050pp::BlockingType::SOFT) {
    // The AGV has to cancel all HARD Actions and stop driving
    SyncNet sn(std::move(begin_ids), mgr->readyPlace());
    ParallelLaunchNet pn(mgr->donePlace(), std::move(end_ids));
    mgr->attachToNet(this->net_);
    if (current_blocking_type == vda5050pp::BlockingType::NONE) {
      // The AGV might be driving
      this->pauseDriving();
      mgr->onExited([this](auto) { this->resumeDriving(); });
    }
    sn.attachToNet(this->net_);
    pn.attachToNet(this->net_);
    // Block currently running hard actions
    if (current_blocking_type == vda5050pp::BlockingType::HARD) {
      this->stopHard();
    }
    // Wait for hard blocking actions to be done, execute in parallel to other actions
    for (auto mgr : intercepted_mgrs) {
      if (mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::HARD) {
        mgr->interceptSequential();
      } else {
        mgr->interceptParallel();
      }
    }
  } else if (action.blockingType == vda5050pp::BlockingType::NONE) {
    // The AGV has to cancel all HARD Actions
    SyncNet sn(std::move(begin_ids), mgr->readyPlace());
    ParallelLaunchNet pn(mgr->donePlace(), std::move(end_ids));
    mgr->attachToNet(this->net_);
    sn.attachToNet(this->net_);
    pn.attachToNet(this->net_);
    // Stop all currently running HARD actions
    if (current_blocking_type == vda5050pp::BlockingType::HARD) {
      this->stopHard();
    }
    // Wait for hard actions to be done, other actions can be run in parallel
    for (auto mgr : intercepted_mgrs) {
      if (mgr->getHandler()->getAction().blockingType == vda5050pp::BlockingType::HARD) {
        mgr->interceptSequential();
      } else {
        mgr->interceptParallel();
      }
    }
  }

  this->action_managers_by_id_[action.actionId] = mgr;
}

void NetManager::pauseDriving() noexcept(true) {
  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    if (mgr->isActive()) {
      mgr->pause();
    }
  }
  for (auto &mgr : this->continuous_navigation_managers_) {
    if (mgr->isActive()) {
      mgr->pause();
    }
  }
}

void NetManager::resumeDriving() noexcept(true) {
  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    if (mgr->isActive()) {
      mgr->resume();
    }
  }
  for (auto &mgr : this->continuous_navigation_managers_) {
    if (mgr->isActive()) {
      mgr->resume();
    }
  }
}

void NetManager::onDrivingChanged(std::function<void(bool driving)> fn) noexcept(true) {
  this->on_driving_changed_ = fn;

  for (auto &mgr : this->continuous_navigation_managers_) {
    mgr->setOnDrivingChanged(fn);
  }

  for (auto &[_, mgr] : this->drive_to_node_managers_by_id_) {
    mgr->onDrivingChanged(fn);
  }
}

void NetManager::onTailReached(std::function<void(void)> fn) noexcept(true) {
  this->on_tail_reached_ = fn;
}

void NetManager::onAllExited(std::function<void(void)> fn) noexcept(true) {
  this->on_all_exited_ = fn;
}

bool NetManager::isAnythingActive() const noexcept(true) { return !this->un_exited_ids_.empty(); }

uint32_t NetManager::nextSeq() noexcept(true) { return this->next_seq_++; }

void NetManager::registerNewTaskManager(TaskManager &mgr,
                                        std::vector<std::string> &&cancel_ids) noexcept(true) {
  // Attach to net
  mgr.attachToNet(this->net_);
  SyncNet sn({this->tail_place_->getID()}, mgr.readyPlace());
  CancelNet cn(std::move(cancel_ids), mgr.donePlace(), *this);
  sn.attachToNet(this->net_);
  cn.attachToNet(this->net_);

  // Exit logic
  this->un_exited_ids_.insert(mgr.exitedPlace());
  mgr.onExited([this](auto id) {
    this->un_exited_ids_.extract(id);
    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
    ha.getLogger().logDebug(
        vda5050pp::core::common::format("Mgr exited: #UnExited={}", this->un_exited_ids_.size()));
    if (this->un_exited_ids_.empty() && this->on_all_exited_ != nullptr) {
      this->on_all_exited_();
    }
  });

  // Places
  this->fail_places_.push_back(this->net_.findPlace(mgr.failedPlace()));
  this->finish_places_.push_back(this->net_.findPlace(mgr.finishedPlace()));
  this->tail_place_->onChange(nullptr);
  this->tail_place_ = this->net_.findPlace(cn.getPlaceID());
  this->tail_place_->onChange([this](auto, auto) {
    if (this->on_tail_reached_ != nullptr) this->on_tail_reached_();
  });
}

const std::shared_ptr<Net::PlaceT> &NetManager::getTailPlace() const noexcept(true) {
  return this->tail_place_;
}

const std::vector<std::shared_ptr<Net::PlaceT>> &NetManager::getFinishPlaces() const
    noexcept(true) {
  return this->finish_places_;
}

const std::vector<std::shared_ptr<Net::PlaceT>> &NetManager::getFailPlaces() const noexcept(true) {
  return this->fail_places_;
}

void NetManager::notifyHorizonChanged() noexcept(true) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  if (ha.isContinuousNavigation() && !this->continuous_navigation_managers_.empty()) {
    this->continuous_navigation_managers_.front()->horizonChanged();
  }
}