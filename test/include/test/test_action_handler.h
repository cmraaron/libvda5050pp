// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// TODO: Short description
//

#ifndef TEST_INCLUDE_TEST_TEST_ACTION_HANDLER
#define TEST_INCLUDE_TEST_TEST_ACTION_HANDLER

#include <map>

#include "vda5050++/interface_agv/action_handler.h"

namespace test {

class TestActionHandler : public vda5050pp::interface_agv::ActionHandler {
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

  TestActionHandler() noexcept(true);
  virtual ~TestActionHandler();
  virtual void start(const vda5050pp::Action &action) override;
  virtual void pause(const vda5050pp::Action &action) override;
  virtual void resume(const vda5050pp::Action &action) override;
  virtual void stop(const vda5050pp::Action &action) override;
};

extern std::map<std::string, std::reference_wrapper<test::TestActionHandler>>
    test_action_handler_by_id;

}  // namespace test

#endif /* TEST_INCLUDE_TEST_TEST_ACTION_HANDLER */
