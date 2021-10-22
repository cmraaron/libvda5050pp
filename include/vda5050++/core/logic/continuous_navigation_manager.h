// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_MANAGER
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_MANAGER

#include <list>
#include <map>
#include <memory>
#include <string>

#include "vda5050++/core/logic/continuous_navigation_single_step_task.h"
#include "vda5050++/interface_agv/continuous_navigation_handler.h"
#include "vda5050++/model/Edge.h"
#include "vda5050++/model/Node.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

class NetManager;

class ContinuousNavigationManager {
private:
  bool finalized_ = false;
  uint32_t first_node_seq_id_;
  uint32_t last_node_seq_id_;
  std::map<uint32_t, std::shared_ptr<ContinuousNavigationSingleStepTask>> single_step_tasks_;
  std::optional<std::reference_wrapper<ContinuousNavigationSingleStepTask>> current_step_;
  std::shared_ptr<vda5050pp::interface_agv::ContinuousNavigationHandler> handler_;
  vda5050pp::interface_agv::Handle &handle_;
  NetManager &net_manager_;
  std::list<vda5050pp::Node> start_nodes_;
  std::list<vda5050pp::Edge> start_edges_;
  std::function<void(bool)> on_driving_changed_;

  void startHandler();
  void destroyHandler();

public:
  void stepDrivingChanged(bool driving) const noexcept(true);
  void stepEntered(ContinuousNavigationSingleStepTask &step);
  void stepExited(ContinuousNavigationSingleStepTask &step);
  void handlerFailed();
  void handlerReachedNode(uint32_t node_seq);
  void handlerStarted();
  void handlerPaused();
  void handlerResumed();

  void setOnDrivingChanged(const std::function<void(bool)> &fn) noexcept(true);

  ContinuousNavigationManager(vda5050pp::interface_agv::Handle &handle, NetManager &net_manager,
                              const vda5050pp::Node &node,
                              std::vector<std::string> &&cancel_action_ids) noexcept(true);

  ContinuousNavigationManager(vda5050pp::interface_agv::Handle &handle, NetManager &net_manager,
                              const vda5050pp::Node &node, const vda5050pp::Edge &edge,
                              std::vector<std::string> &&cancel_action_ids) noexcept(true);

  void append(const vda5050pp::Node &node, const std::optional<vda5050pp::Edge> &edge,
              std::vector<std::string> &&cancel_action_ids) noexcept(true);

  void commitAppendings();
  void finalize();
  bool isFinalized() const;
  void pause();
  void resume();
  void stop();

  void horizonChanged();

  bool cancel() const noexcept(true);
  bool intercept() noexcept(true);
  bool isActive() const noexcept(true);
  std::shared_ptr<ContinuousNavigationSingleStepTask> getInterceptedMgr() noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_MANAGER */
