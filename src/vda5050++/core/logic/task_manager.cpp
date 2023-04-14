// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "vda5050++/core/logic/task_manager.h"

#include "vda5050++/core/interface_agv/handle_accessor.h"

using namespace vda5050pp::core::logic;

TaskManager::TaskManager(vda5050pp::interface_agv::Handle &handle, SeqNrT seq)
    : handle_(handle), seq_(seq) {}

void TaskManager::attachToNet(Net &composed_net) noexcept(false) {
  Net transaction_net;

  // Create all places for the partial net
  transaction_net.addPlace({LogicTaskNetTypes::k_ready, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_enabled, this->seq_}, 1);
  transaction_net.addPlace({LogicTaskNetTypes::k_disabled, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_entered, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_exited, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_initializing, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_running, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_paused, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_finished, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_failed, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_done, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_pre_done, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_any_failed, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercepted, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercepted_parallel, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercepted_sequential, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercept_sync, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_un_intercepted, this->seq_}, 1);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercepting_begin, this->seq_}, 0);
  transaction_net.addPlace({LogicTaskNetTypes::k_intercepting_end, this->seq_}, 0);

  // Create all transitions from ready to finished over paused
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_start, this->seq_},               // ID
       {{{LogicTaskNetTypes::k_ready, this->seq_}, 1},         // from ready
        {{LogicTaskNetTypes::k_enabled, this->seq_}, 1}},      // from enabled
       {{{LogicTaskNetTypes::k_initializing, this->seq_}, 1},  // to initializing
        {{LogicTaskNetTypes::k_entered, this->seq_}, 1}}});    // to entered
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_started, this->seq_},                          // ID
       {{{LogicTaskNetTypes::k_initializing, this->seq_}, 1}},              // from initializing
       {{{LogicTaskNetTypes::k_running, this->seq_}, 1}}});                 // to running
  transaction_net.addTransition({{LogicTaskNetTypes::k_pause, this->seq_},  // ID
                                 {{{LogicTaskNetTypes::k_running, this->seq_}, 1}},  // from running
                                 {{{LogicTaskNetTypes::k_paused, this->seq_}, 1}}});  // to pausing
  transaction_net.addTransition({{LogicTaskNetTypes::k_resume, this->seq_},           // ID
                                 {{{LogicTaskNetTypes::k_paused, this->seq_}, 1}},    // from paused
                                 {{{LogicTaskNetTypes::k_running, this->seq_}, 1}}});  // to running

  // Create all fail transitions
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_initializing_fail, this->seq_},    // ID
       {{{LogicTaskNetTypes::k_initializing, this->seq_}, 1}},  // from initializing
       {{{LogicTaskNetTypes::k_any_failed, this->seq_}, 1}}});  // to any_failed
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_running_fail, this->seq_},         // ID
       {{{LogicTaskNetTypes::k_running, this->seq_}, 1}},       // from running
       {{{LogicTaskNetTypes::k_any_failed, this->seq_}, 1}}});  // to any_failed
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_paused_fail, this->seq_},          // ID
       {{{LogicTaskNetTypes::k_paused, this->seq_}, 1}},        // from paused
       {{{LogicTaskNetTypes::k_any_failed, this->seq_}, 1}}});  // to any_failed

  // done transitions
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_fail, this->seq_},               // ID
       {{{LogicTaskNetTypes::k_any_failed, this->seq_}, 1}},  // from any_failed
       {{{LogicTaskNetTypes::k_pre_done, this->seq_}, 1},     // to pre_done
        {{LogicTaskNetTypes::k_failed, this->seq_}, 1},       // to failed
        {{LogicTaskNetTypes::k_exited, this->seq_}, 1}}});    // to exited
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_finish, this->seq_},           // ID
       {{{LogicTaskNetTypes::k_running, this->seq_}, 1}},   // from running:
       {{{LogicTaskNetTypes::k_pre_done, this->seq_}, 1},   // to pre_done
        {{LogicTaskNetTypes::k_finished, this->seq_}, 1},   // to finished
        {{LogicTaskNetTypes::k_exited, this->seq_}, 1}}});  // to exiteded

  // disable and skip transitions
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_disable, this->seq_},            // ID
       {{{LogicTaskNetTypes::k_enabled, this->seq_}, 1}},     // from enabled
       {{{LogicTaskNetTypes::k_disabled, this->seq_}, 1}}});  // to disabled
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_skip, this->seq_},                 // ID
       {{{LogicTaskNetTypes::k_disabled, this->seq_}, 1},       // from ready
        {{LogicTaskNetTypes::k_ready, this->seq_}, 1}},         // from enabled
       {{{LogicTaskNetTypes::k_any_failed, this->seq_}, 1}}});  // to disabled

  // normal flow
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_pre_to_done, this->seq_},            // ID
       {{{LogicTaskNetTypes::k_pre_done, this->seq_}, 1},         // from pre_done
        {{LogicTaskNetTypes::k_un_intercepted, this->seq_}, 1}},  // from un_intercepted
       {{{LogicTaskNetTypes::k_done, this->seq_}, 1}}});          // to done
  // intercept
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_intercept, this->seq_},              // ID
       {{{LogicTaskNetTypes::k_un_intercepted, this->seq_}, 1}},  // from un_intercepted
       {{{LogicTaskNetTypes::k_intercepted, this->seq_}, 1}}});   // to intercepted
  // sequential flow
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_intercept_sequential, this->seq_},  // ID
       {{{LogicTaskNetTypes::k_intercepted, this->seq_}, 1}},    // from un_intercepted
       {{{LogicTaskNetTypes::k_intercepted_sequential, this->seq_},
         1}}});  // to intercepted_sequential
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_sequential_to_intercepting, this->seq_},  // ID
       {{{LogicTaskNetTypes::k_pre_done, this->seq_}, 1},              // from pre_done
        {{LogicTaskNetTypes::k_intercepted_sequential, this->seq_},
         1}},                                                        // from intercepted_sequential
       {{{LogicTaskNetTypes::k_intercepting_begin, this->seq_}, 1},  // to intercepting_begin
        {{LogicTaskNetTypes::k_intercept_sync, this->seq_}, 1}}});   // to intercept_sync
  // parallel flow
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_intercept_parallel, this->seq_},           // ID
       {{{LogicTaskNetTypes::k_intercepted, this->seq_}, 1}},           // from un_intercepted
       {{{LogicTaskNetTypes::k_intercepted_parallel, this->seq_}, 1},   // to intercepted_parallel
        {{LogicTaskNetTypes::k_intercepting_begin, this->seq_}, 1}}});  // to intercepting_begin
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_parallel_to_intercepting, this->seq_},    // ID
       {{{LogicTaskNetTypes::k_intercepted_parallel, this->seq_}, 1},  // from intercepted_parallel
        {{LogicTaskNetTypes::k_pre_done, this->seq_}, 1}},             // from pre_done
       {{{LogicTaskNetTypes::k_intercept_sync, this->seq_}, 1}}});     // to intercept_sync
  // intercepted done flow
  transaction_net.addTransition(
      {{LogicTaskNetTypes::k_intercepting_to_done, this->seq_},    // ID
       {{{LogicTaskNetTypes::k_intercepting_end, this->seq_}, 1},  // from intercepting_end
        {{LogicTaskNetTypes::k_intercept_sync, this->seq_}, 1}},   // from intercept_sync
       {{{LogicTaskNetTypes::k_done, this->seq_}, 1}}});           // to done

  // Merge transaction net to the composed global net
  composed_net.merge(std::move(transaction_net), {});

  auto if_reached_then = [this](auto fn) {
    return [this, fn](auto &place, auto prev) {
      if (place.getTokens() == 1 && prev == 0) {
        this->logPlaceReached(place);
        fn();
      }
    };
  };

  // Place hooks
  composed_net.findPlace({LogicTaskNetTypes::k_initializing, this->seq_})
      ->onChange(if_reached_then([this] { this->taskInitialize(); }));
  composed_net.findPlace({LogicTaskNetTypes::k_running, this->seq_})
      ->onChange(if_reached_then([this] { this->taskRunning(); }));
  composed_net.findPlace({LogicTaskNetTypes::k_paused, this->seq_})
      ->onChange(if_reached_then([this] { this->taskPaused(); }));
  composed_net.findPlace({LogicTaskNetTypes::k_finished, this->seq_})
      ->onChange(if_reached_then([this] { this->taskFinished(); }));
  composed_net.findPlace({LogicTaskNetTypes::k_failed, this->seq_})
      ->onChange(if_reached_then([this] { this->taskFailed(); }));

  // Store some transitions for manual firing
  this->transition_initializing_fail_ =
      composed_net.findTransition({LogicTaskNetTypes::k_initializing_fail, this->seq_});
  this->transition_running_fail_ =
      composed_net.findTransition({LogicTaskNetTypes::k_running_fail, this->seq_});
  this->transition_paused_fail_ =
      composed_net.findTransition({LogicTaskNetTypes::k_paused_fail, this->seq_});
  this->transition_started_ =
      composed_net.findTransition({LogicTaskNetTypes::k_started, this->seq_});
  this->transition_pause_ = composed_net.findTransition({LogicTaskNetTypes::k_pause, this->seq_});
  this->transition_resume_ = composed_net.findTransition({LogicTaskNetTypes::k_resume, this->seq_});
  this->transition_finish_ = composed_net.findTransition({LogicTaskNetTypes::k_finish, this->seq_});
  this->transition_fail_ = composed_net.findTransition({LogicTaskNetTypes::k_fail, this->seq_});
  this->transition_disable_ =
      composed_net.findTransition({LogicTaskNetTypes::k_disable, this->seq_});
  this->transition_intercept_ =
      composed_net.findTransition({LogicTaskNetTypes::k_intercept, this->seq_});
  this->transition_intercept_parallel_ =
      composed_net.findTransition({LogicTaskNetTypes::k_intercept_parallel, this->seq_});
  this->transition_intercept_sequential_ =
      composed_net.findTransition({LogicTaskNetTypes::k_intercept_sequential, this->seq_});

  // Store some places for observation
  this->place_failed_ = composed_net.findPlace({LogicTaskNetTypes::k_failed, this->seq_});
  this->place_finished_ = composed_net.findPlace({LogicTaskNetTypes::k_finished, this->seq_});
  this->place_paused_ = composed_net.findPlace({LogicTaskNetTypes::k_paused, this->seq_});
  this->place_running_ = composed_net.findPlace({LogicTaskNetTypes::k_running, this->seq_});
  this->place_initializing_ =
      composed_net.findPlace({LogicTaskNetTypes::k_initializing, this->seq_});
  this->place_entered_ = composed_net.findPlace({LogicTaskNetTypes::k_entered, this->seq_});
  this->place_exited_ = composed_net.findPlace({LogicTaskNetTypes::k_exited, this->seq_});

  // auto firing
  this->transition_fail_->autoFire();
  composed_net.findTransition({LogicTaskNetTypes::k_skip, this->seq_})->autoFire();
  composed_net.findTransition({LogicTaskNetTypes::k_intercepting_to_done, this->seq_})->autoFire();
  composed_net.findTransition({LogicTaskNetTypes::k_parallel_to_intercepting, this->seq_})
      ->autoFire();
  composed_net.findTransition({LogicTaskNetTypes::k_sequential_to_intercepting, this->seq_})
      ->autoFire();
  composed_net.findTransition({LogicTaskNetTypes::k_pre_to_done, this->seq_})->autoFire();

  // This enables this branch of the net, for safety enable it last
  composed_net.findTransition({LogicTaskNetTypes::k_start, this->seq_})->autoFire();
}

LogicTaskNetID TaskManager::readyPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_ready, this->seq_};
}
LogicTaskNetID TaskManager::donePlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_done, this->seq_};
}
LogicTaskNetID TaskManager::failedPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_failed, this->seq_};
}
LogicTaskNetID TaskManager::finishedPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_finished, this->seq_};
}

LogicTaskNetID TaskManager::exitedPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_exited, this->seq_};
}

LogicTaskNetID TaskManager::interceptingBeginPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_intercepting_begin, this->seq_};
}

LogicTaskNetID TaskManager::interceptingEndPlace() const noexcept(true) {
  return {LogicTaskNetTypes::k_intercepting_end, this->seq_};
}

void TaskManager::onExited(std::function<void(LogicTaskNetID exit_id)> fn) noexcept(true) {
  this->place_exited_->onChange([this, fn](const auto &place, auto before) {
    if (place.getTokens() == 1 && before == 0) {
      this->logPlaceReached(place);
      if (fn != nullptr) {
        fn(place.getID());
      }
    }
  });
}

bool TaskManager::started() noexcept(true) {
  return this->logTransition("started()", this->transition_started_->deepFire());
}
bool TaskManager::finished() noexcept(true) {
  return this->logTransition("finished()", this->transition_finish_->deepFire());
}
bool TaskManager::failed() noexcept(true) {
  return this->logTransition("failed()", this->transition_paused_fail_->deepFire() ||
                                             this->transition_initializing_fail_->deepFire() ||
                                             this->transition_running_fail_->deepFire());
}

bool TaskManager::paused() noexcept(true) {
  return this->logTransition("pause()", this->transition_pause_->deepFire());
}

bool TaskManager::resumed() noexcept(true) {
  return this->logTransition("resume()", this->transition_resume_->deepFire());
}

bool TaskManager::isRunning() noexcept(true) { return this->place_running_->getTokens() == 1; }
bool TaskManager::isPaused() noexcept(true) { return this->place_paused_->getTokens() == 1; }
bool TaskManager::isFailed() noexcept(true) { return this->place_failed_->getTokens() == 1; }
bool TaskManager::isFinished() noexcept(true) { return this->place_finished_->getTokens() == 1; }
bool TaskManager::isDone() noexcept(true) { return this->isFailed() || this->isFinished(); }
bool TaskManager::isActive() noexcept(true) {
  return this->place_entered_->getTokens() == 1 && !this->isDone();
}
bool TaskManager::isInitializing() noexcept(true) {
  return this->place_initializing_->getTokens() == 1;
}
bool TaskManager::cancel() noexcept(true) {
  return this->logTransition("disable()", this->transition_disable_->deepFire());
}

bool TaskManager::intercept() noexcept(true) {
  return this->logTransition("intercept()", this->transition_intercept_->fire());
}

bool TaskManager::interceptParallel() noexcept(true) {
  return this->logTransition("interceptParallel()",
                             this->transition_intercept_parallel_->deepFire());
}
bool TaskManager::interceptSequential() noexcept(true) {
  return this->logTransition("interceptSequential()",
                             this->transition_intercept_sequential_->deepFire());
}

SeqNrT TaskManager::getSeq() const noexcept(true) { return this->seq_; }