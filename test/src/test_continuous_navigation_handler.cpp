// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/test_continuous_navigation_handler.h"

#include <functional>

using namespace test;

std::map<uint32_t, std::reference_wrapper<TestContinuousNavigationHandler>>
    test::continuous_navigation_handler_by_seq = {};

bool TestContinuousNavigationHandler::auto_fail_on_stop_;
bool TestContinuousNavigationHandler::auto_start_;
bool TestContinuousNavigationHandler::auto_pause_;
bool TestContinuousNavigationHandler::auto_resume_;

int TestContinuousNavigationHandler::timesStartCalled() const noexcept(true) {
  return this->times_start_called_;
}
int TestContinuousNavigationHandler::timesPauseCalled() const noexcept(true) {
  return this->times_pause_called_;
}
int TestContinuousNavigationHandler::timesResumeCalled() const noexcept(true) {
  return this->times_resume_called_;
}
int TestContinuousNavigationHandler::timesStopCalled() const noexcept(true) {
  return this->times_stop_called_;
}
int TestContinuousNavigationHandler::timesBaseIncreasedCalled() const noexcept(true) {
  return this->time_base_increased_called_;
}
int TestContinuousNavigationHandler::timesHorizonUpdatedCalled() const noexcept(true) {
  return this->time_horizon_updated_called_;
}

void TestContinuousNavigationHandler::setAutoFailOnStop(bool auto_fail_on_stop) {
  TestContinuousNavigationHandler::auto_fail_on_stop_ = auto_fail_on_stop;
}
void TestContinuousNavigationHandler::setAutoStart(bool auto_start) {
  TestContinuousNavigationHandler::auto_start_ = auto_start;
}
void TestContinuousNavigationHandler::setAutoPause(bool auto_pause) {
  TestContinuousNavigationHandler::auto_pause_ = auto_pause;
}
void TestContinuousNavigationHandler::setAutoResume(bool auto_resume) {
  TestContinuousNavigationHandler::auto_resume_ = auto_resume;
}

void TestContinuousNavigationHandler::doFailed() { this->failed(); }
void TestContinuousNavigationHandler::doStarted() { this->started(); }
void TestContinuousNavigationHandler::doPaused() { this->paused(); }
void TestContinuousNavigationHandler::doResumed() { this->resumed(); }
void TestContinuousNavigationHandler::doNodeReached(uint32_t node_seq) {
  this->setNodeReached(node_seq);
}

const std::list<vda5050pp::Node> &TestContinuousNavigationHandler::doGetBaseNodes() {
  return this->getBaseNodes();
}

const std::list<vda5050pp::Edge> &TestContinuousNavigationHandler::doGetBaseEdges() {
  return this->getBaseEdges();
}

TestContinuousNavigationHandler::TestContinuousNavigationHandler()
    : times_start_called_(0),
      times_pause_called_(0),
      times_resume_called_(0),
      times_stop_called_(0),
      time_base_increased_called_(0) {}
TestContinuousNavigationHandler::~TestContinuousNavigationHandler() {}

void TestContinuousNavigationHandler::horizonUpdated(const std::list<vda5050pp::Node> &,
                                                     const std::list<vda5050pp::Edge> &) {
  this->time_horizon_updated_called_++;
}
void TestContinuousNavigationHandler::baseIncreased(
    const std::list<vda5050pp::Node> &base_append_nodes, const std::list<vda5050pp::Edge> &) {
  this->time_base_increased_called_++;
  for (const auto &node : base_append_nodes) {
    test::continuous_navigation_handler_by_seq.emplace(node.sequenceId, std::ref(*this));
  }
}
void TestContinuousNavigationHandler::start(const std::list<vda5050pp::Node> &base_nodes,
                                            const std::list<vda5050pp::Edge> &) {
  this->times_start_called_++;
  for (const auto &node : base_nodes) {
    test::continuous_navigation_handler_by_seq.emplace(node.sequenceId, std::ref(*this));
  }

  if (this->auto_start_) {
    this->started();
  }
}
void TestContinuousNavigationHandler::pause() {
  this->times_pause_called_++;
  if (this->auto_pause_) {
    this->paused();
  }
}
void TestContinuousNavigationHandler::resume() {
  this->times_resume_called_++;
  if (this->auto_resume_) {
    this->resume();
  }
}
void TestContinuousNavigationHandler::stop() {
  this->times_stop_called_++;

  if (this->auto_fail_on_stop_) {
    this->failed();
  }
}