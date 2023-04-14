// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// TODO: Short description
//

#ifndef TEST_INCLUDE_TEST_TEST_STEP_BASED_NAVIGATION_HANDLER
#define TEST_INCLUDE_TEST_TEST_STEP_BASED_NAVIGATION_HANDLER

#include <map>

#include "vda5050++/interface_agv/step_based_navigation_handler.h"

namespace test {

class TestStepBasedNavigationHandler : public vda5050pp::interface_agv::StepBasedNavigationHandler {
private:
  int times_start_called_;
  int times_pause_called_;
  int times_resume_called_;
  int times_stop_called_;

  static bool auto_fail_on_stop_;
  static bool auto_finish_on_stop_;
  static bool auto_start_;
  static bool auto_pause_;
  static bool auto_resume_;

public:
  int timesStartCalled() const noexcept(true);
  int timesPauseCalled() const noexcept(true);
  int timesResumeCalled() const noexcept(true);
  int timesStopCalled() const noexcept(true);

  static void setAutoFailOnStop(bool auto_fail_on_stop);
  static void setAutoFinishOnStop(bool auto_finish_on_stop);
  static void setAutoStart(bool auto_start);
  static void setAutoPause(bool auto_pause);
  static void setAutoResume(bool auto_resume);

  void doFinished();
  void doFailed();
  void doStarted();
  void doPaused();
  void doResumed();

  TestStepBasedNavigationHandler();
  virtual ~TestStepBasedNavigationHandler();
  virtual void start(const std::optional<vda5050pp::Edge> &via_edge,
                     const vda5050pp::Node &goal_node) override;
  virtual void pause() override;
  virtual void resume() override;
  virtual void stop() override;
};

extern std::map<uint32_t, std::reference_wrapper<TestStepBasedNavigationHandler>>
    navigate_to_node_handler_by_seq;

}  // namespace test

#endif /* TEST_INCLUDE_TEST_TEST_STEP_BASED_NAVIGATION_HANDLER */
