// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// TODO: Short description
//

#ifndef TEST_INCLUDE_TEST_TEST_CONTINUOUS_NAVIGATION_HANDLER
#define TEST_INCLUDE_TEST_TEST_CONTINUOUS_NAVIGATION_HANDLER

#include <map>

#include "vda5050++/interface_agv/continuous_navigation_handler.h"

namespace test {

class TestContinuousNavigationHandler
    : public vda5050pp::interface_agv::ContinuousNavigationHandler {
private:
  int times_start_called_;
  int times_pause_called_;
  int times_resume_called_;
  int times_stop_called_;
  int time_base_increased_called_;
  int time_horizon_updated_called_;

  static bool auto_fail_on_stop_;
  static bool auto_start_;
  static bool auto_pause_;
  static bool auto_resume_;

public:
  int timesStartCalled() const noexcept(true);
  int timesPauseCalled() const noexcept(true);
  int timesResumeCalled() const noexcept(true);
  int timesStopCalled() const noexcept(true);
  int timesBaseIncreasedCalled() const noexcept(true);
  int timesHorizonUpdatedCalled() const noexcept(true);

  static void setAutoFailOnStop(bool auto_fail_on_stop);
  static void setAutoStart(bool auto_start);
  static void setAutoPause(bool auto_pause);
  static void setAutoResume(bool auto_resume);

  void doFailed();
  void doStarted();
  void doPaused();
  void doResumed();
  void doNodeReached(uint32_t node_seq);
  const std::list<vda5050pp::Node> &doGetBaseNodes();
  const std::list<vda5050pp::Edge> &doGetBaseEdges();

  TestContinuousNavigationHandler();
  virtual ~TestContinuousNavigationHandler();
  virtual void horizonUpdated(const std::list<vda5050pp::Node> &new_horizon_nodes,
                              const std::list<vda5050pp::Edge> &new_horizon_edges) override;
  virtual void baseIncreased(const std::list<vda5050pp::Node> &base_append_nodes,
                             const std::list<vda5050pp::Edge> &base_append_edges) override;
  virtual void start(const std::list<vda5050pp::Node> &base_nodes,
                     const std::list<vda5050pp::Edge> &edge_edges) override;
  virtual void pause() override;
  virtual void resume() override;
  virtual void stop() override;
};

extern std::map<uint32_t, std::reference_wrapper<TestContinuousNavigationHandler>>
    continuous_navigation_handler_by_seq;

}  // namespace test

#endif /* TEST_INCLUDE_TEST_TEST_CONTINUOUS_NAVIGATION_HANDLER */
