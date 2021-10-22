// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/test_step_based_navigation_handler.h"

#include <functional>

bool test::TestStepBasedNavigationHandler::auto_fail_on_stop_ = false;
bool test::TestStepBasedNavigationHandler::auto_finish_on_stop_ = false;
bool test::TestStepBasedNavigationHandler::auto_start_ = false;
bool test::TestStepBasedNavigationHandler::auto_pause_ = false;
bool test::TestStepBasedNavigationHandler::auto_resume_ = false;

int test::TestStepBasedNavigationHandler::timesStartCalled() const noexcept(true) {
  return this->times_start_called_;
}
int test::TestStepBasedNavigationHandler::timesPauseCalled() const noexcept(true) {
  return this->times_pause_called_;
}
int test::TestStepBasedNavigationHandler::timesResumeCalled() const noexcept(true) {
  return this->times_resume_called_;
}
int test::TestStepBasedNavigationHandler::timesStopCalled() const noexcept(true) {
  return this->times_stop_called_;
}

void test::TestStepBasedNavigationHandler::setAutoFailOnStop(bool auto_fail_on_stop) {
  test::TestStepBasedNavigationHandler::auto_fail_on_stop_ = auto_fail_on_stop;
}
void test::TestStepBasedNavigationHandler::setAutoFinishOnStop(bool auto_finish_on_stop) {
  test::TestStepBasedNavigationHandler::auto_finish_on_stop_ = auto_finish_on_stop;
}
void test::TestStepBasedNavigationHandler::setAutoStart(bool auto_start) {
  test::TestStepBasedNavigationHandler::auto_start_ = auto_start;
}
void test::TestStepBasedNavigationHandler::setAutoPause(bool auto_pause) {
  test::TestStepBasedNavigationHandler::auto_pause_ = auto_pause;
}
void test::TestStepBasedNavigationHandler::setAutoResume(bool auto_resume) {
  test::TestStepBasedNavigationHandler::auto_resume_ = auto_resume;
}

void test::TestStepBasedNavigationHandler::doFinished() { this->finished(); }
void test::TestStepBasedNavigationHandler::doFailed() { this->failed(); }
void test::TestStepBasedNavigationHandler::doStarted() { this->started(); }
void test::TestStepBasedNavigationHandler::doPaused() { this->paused(); }
void test::TestStepBasedNavigationHandler::doResumed() { this->resumed(); }

test::TestStepBasedNavigationHandler::TestStepBasedNavigationHandler()
    : times_start_called_(0),
      times_pause_called_(0),
      times_resume_called_(0),
      times_stop_called_(0) {}

test::TestStepBasedNavigationHandler::~TestStepBasedNavigationHandler() {
  try {
    test::navigate_to_node_handler_by_seq.extract(this->getGoalNode().sequenceId);
  } catch (const std::exception &) {
  }
}

void test::TestStepBasedNavigationHandler::start(const std::optional<vda5050pp::Edge> &,
                                                 const vda5050pp::Node &goal_node) {
  test::navigate_to_node_handler_by_seq.emplace(goal_node.sequenceId, std::ref(*this));
  this->times_start_called_++;
  if (TestStepBasedNavigationHandler::auto_start_) {
    this->started();
  }
}

void test::TestStepBasedNavigationHandler::pause() {
  this->times_pause_called_++;
  if (TestStepBasedNavigationHandler::auto_pause_) {
    this->paused();
  }
}
void test::TestStepBasedNavigationHandler::resume() {
  this->times_resume_called_++;

  if (TestStepBasedNavigationHandler::auto_resume_) {
    this->resumed();
  }
}
void test::TestStepBasedNavigationHandler::stop() {
  this->times_stop_called_++;
  if (TestStepBasedNavigationHandler::auto_fail_on_stop_) {
    this->failed();
  } else if (TestStepBasedNavigationHandler::auto_finish_on_stop_) {
    this->finished();
  }
}

std::map<uint32_t, std::reference_wrapper<test::TestStepBasedNavigationHandler>>
    test::navigate_to_node_handler_by_seq;