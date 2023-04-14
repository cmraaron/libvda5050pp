// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "test/test_action_handler.h"

#include <functional>

std::map<std::string, std::reference_wrapper<test::TestActionHandler>>
    test::test_action_handler_by_id;

bool test::TestActionHandler::auto_fail_on_stop_ = false;
bool test::TestActionHandler::auto_finish_on_stop_ = false;
bool test::TestActionHandler::auto_start_ = false;
bool test::TestActionHandler::auto_pause_ = false;
bool test::TestActionHandler::auto_resume_ = false;

int test::TestActionHandler::timesStartCalled() const noexcept(true) {
  return this->times_start_called_;
}
int test::TestActionHandler::timesPauseCalled() const noexcept(true) {
  return this->times_pause_called_;
}
int test::TestActionHandler::timesResumeCalled() const noexcept(true) {
  return this->times_resume_called_;
}
int test::TestActionHandler::timesStopCalled() const noexcept(true) {
  return this->times_stop_called_;
}

void test::TestActionHandler::doFinished() { this->finished(); }
void test::TestActionHandler::doFailed() { this->failed(); }
void test::TestActionHandler::doStarted() { this->started(); }
void test::TestActionHandler::doPaused() { this->paused(); }
void test::TestActionHandler::doResumed() { this->resumed(); }

void test::TestActionHandler::setAutoFailOnStop(bool auto_fail_on_stop) {
  TestActionHandler::auto_fail_on_stop_ = auto_fail_on_stop;
}
void test::TestActionHandler::setAutoFinishOnStop(bool auto_finish_on_stop) {
  TestActionHandler::auto_finish_on_stop_ = auto_finish_on_stop;
}
void test::TestActionHandler::setAutoStart(bool auto_start) {
  TestActionHandler::auto_start_ = auto_start;
}
void test::TestActionHandler::setAutoPause(bool auto_pause) {
  TestActionHandler::auto_pause_ = auto_pause;
}
void test::TestActionHandler::setAutoResume(bool auto_resume) {
  TestActionHandler::auto_resume_ = auto_resume;
}

test::TestActionHandler::TestActionHandler() noexcept(true)
    : times_start_called_(0),
      times_pause_called_(0),
      times_resume_called_(0),
      times_stop_called_(0) {}

test::TestActionHandler::~TestActionHandler() {
  try {
    test::test_action_handler_by_id.extract(this->getAction().actionId);
  } catch (const std::exception &) {
  }
}

void test::TestActionHandler::start(const vda5050pp::Action &action) {
  test::test_action_handler_by_id.emplace(action.actionId, std::ref(*this));
  this->times_start_called_++;
  if (TestActionHandler::auto_start_) {
    this->started();
  }
}
void test::TestActionHandler::pause(const vda5050pp::Action &) {
  this->times_pause_called_++;
  if (TestActionHandler::auto_pause_) {
    this->paused();
  }
}
void test::TestActionHandler::resume(const vda5050pp::Action &) {
  this->times_resume_called_++;
  if (TestActionHandler::auto_resume_) {
    this->resumed();
  }
}
void test::TestActionHandler::stop(const vda5050pp::Action &) {
  this->times_stop_called_++;
  if (TestActionHandler::auto_fail_on_stop_) {
    this->failed();
  } else if (TestActionHandler::auto_finish_on_stop_) {
    this->finished();
  }
}