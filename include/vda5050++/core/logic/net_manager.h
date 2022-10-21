// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the NetManager declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_NET_MANAGER
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_NET_MANAGER

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "../../model/Order.h"
#include "action_manager.h"
#include "continuous_navigation_manager.h"
#include "drive_to_node_manager.h"
#include "types.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
///\brief Manages the logic PTN
///
class NetManager {
private:
  Net net_;
  SeqNrT next_seq_ = 0;

  std::shared_ptr<Net::PlaceT> tail_place_;
  std::vector<std::shared_ptr<Net::PlaceT>> fail_places_;
  std::vector<std::shared_ptr<Net::PlaceT>> finish_places_;
  std::set<LogicTaskNetID> un_exited_ids_;

  std::list<std::shared_ptr<ContinuousNavigationManager>> continuous_navigation_managers_;
  std::map<std::string, std::shared_ptr<ActionManager>, std::less<>> action_managers_by_id_;
  std::map<uint32_t, std::shared_ptr<DriveToNodeManager>> drive_to_node_managers_by_id_;

  vda5050pp::interface_agv::Handle &handle_;

  std::function<void(bool)> on_driving_changed_;
  std::function<void(void)> on_tail_reached_;
  std::function<void(void)> on_all_exited_;

  ///
  ///\brief Insert a Timestep with parallel actions
  ///       SOFT and HARD actions will be synchronized before the next timestep
  ///
  ///\param action_ids all actions to run concurrently
  ///\param force_sync synchronize all actions (used when HARD actions will be scheduled next)
  ///
  void insertTimeStepAction(const std::vector<std::string> &action_ids,
                            bool force_sync) noexcept(false);

  ///
  ///\brief Insert a Timestep for driving to a Node
  ///
  ///\param node the node to drive to
  ///\param edge the via-edge
  ///\param cancel_action_ids  all actions that will be canceled once the Node is reached
  ///
  void insertTimeStepDriveToNode(const vda5050pp::Node &node, const vda5050pp::Edge &edge,
                                 std::vector<std::string> &&cancel_action_ids) noexcept(false);

  ///
  ///\brief Interpret *only* the actions of a Node.
  ///       No driving step will be inserted, because this will only be called
  ///       for the first node of each order.
  ///
  ///\param node  the node to interpret
  ///
  void interpretNode(const vda5050pp::Node &node) noexcept(false);

  ///
  ///\brief Interpret in the following order:
  /// 1. Actions of the Edge (not syncing NONE-Blocking)
  /// 2. Drive from Edge to Node
  /// 3. Interpret the Node
  ///
  ///\param edge
  ///\param node
  ///
  void interpretEdgeThenNode(const vda5050pp::Edge &edge,
                             const vda5050pp::Node &node) noexcept(false);

public:
  ///
  ///\brief Construct a new Net Manager object
  ///
  ///\param handle library handle
  ///
  explicit NetManager(vda5050pp::interface_agv::Handle &handle) noexcept(true);

  ///
  ///\brief Extend the internal PTN based on the internal state.
  ///       When the PTN was extended, then the state.graph_interpreted_seq
  ///       is incremented
  ///
  void interpret() noexcept(false);

  ///
  ///\brief Clear the net
  ///
  void clear() noexcept(true);

  ///
  ///\brief Tick the net once to fire automatic transitions (Time steps)
  ///
  void tick() noexcept(true);

  ///
  ///\brief Pause the action identified by the given ID
  ///
  ///\param action_id the actions ID
  ///\throws std::invalid_argument on invalid ids
  ///\throws std::logic_error when the action is not pausable
  ///
  void pauseAction(const std::string &action_id) noexcept(false);

  ///
  ///\brief Resume the action identified by the given ID
  ///
  ///\param action_id the actions ID
  ///\throws std::invalid_argument on invalid ids
  ///\throws std::logic_error when the action is not resumable
  ///
  void resumeAction(const std::string &action_id) noexcept(false);

  ///
  ///\brief Stop the action identified by the given ID
  ///
  ///\param action_id the actions ID
  ///\throws std::invalid_argument on invalid ids
  ///\throws std::logic_error when the action is not stoppable
  ///
  void stopAction(const std::string &action_id) noexcept(false);

  ///
  /// \brief Is the action active?
  ///
  /// \param action_id the actions ID
  /// \return is active?
  /// \throws std::invalid_argument on invalid ids
  ///
  bool isActionActive(const std::string &action_id) noexcept(false);

  ///
  ///\brief Pause all running Tasks
  ///
  void pauseAllRunningActions() noexcept(true);

  ///
  ///\brief Resume all paused Tasks
  ///
  void resumeAllPausedActions() noexcept(true);

  ///
  ///\brief Stop all active Tasks
  ///
  void stopAllActive() noexcept(true);

  ///
  ///\brief cancel all pending tasks and stop all running tasks
  ///
  void cancelAll() noexcept(true);

  ///
  ///\brief cancel all pending tasks
  ///
  void cancelAllPending() noexcept(true);

  void onDrivingChanged(std::function<void(bool driving)> fn) noexcept(true);

  ///
  ///\brief Notify when the tail place was reached
  ///
  ///\param fn
  ///
  void onTailReached(std::function<void(void)> fn) noexcept(true);

  ///
  /// \brief Notify when all tasks were exited
  ///
  /// \param fn
  ///
  void onAllExited(std::function<void(void)> fn) noexcept(true);

  ///
  /// \brief Check if there is any active task (is any task un-exited)?
  ///
  /// \return anything active?
  ///
  bool isAnythingActive() const noexcept(true);

  ///
  /// \brief Intercept all active actions
  ///
  /// \return std::vector<std::shared_ptr<ActionManager>> all intercepted action managers
  ///
  std::vector<std::shared_ptr<ActionManager>> interceptAllActive() noexcept(true);

  ///
  /// \brief Intercept all active driving managers
  ///
  /// \return std::shared_ptr<TaskManager> the intercepted driving manager or nullptr
  ///
  std::shared_ptr<TaskManager> interceptDriving() noexcept(true);

  /// \brief Stop all currently running HARD blocking actions
  void stopHard() noexcept(true);

  /// \brief Stop all currently running SOFT blocking actions
  void stopSoft() noexcept(true);

  /// \brief Stop all currently running NONE blocking actions
  void stopNone() noexcept(true);

  /// \brief pause all driving managers
  void pauseDriving() noexcept(true);

  /// \brief pause all driving managers
  void resumeDriving() noexcept(true);

  uint32_t nextSeq() noexcept(true);

  void registerNewTaskManager(TaskManager &mgr,
                              std::vector<std::string> &&cancel_ids) noexcept(true);

  ///
  /// \brief Intercept the current flow with an action (does not insert action into the state)
  ///
  /// \param action the action to execute
  ///
  void interceptWithAction(const vda5050pp::Action &action) noexcept(true);

  ///
  ///\brief Get the current Tail Place (the last Place of the interpreted order)
  ///
  ///\return const std::shared_ptr<Net::PlaceT>&
  ///
  const std::shared_ptr<Net::PlaceT> &getTailPlace() const noexcept(true);

  ///
  ///\brief Get all finished Places of Tasks
  ///
  ///\return const std::vector<std::shared_ptr<Net::PlaceT>>&
  ///
  const std::vector<std::shared_ptr<Net::PlaceT>> &getFinishPlaces() const noexcept(true);

  ///
  ///\brief Get the Fail Places of Tasks
  ///
  ///\return const std::vector<std::shared_ptr<Net::PlaceT>>&
  ///
  const std::vector<std::shared_ptr<Net::PlaceT>> &getFailPlaces() const noexcept(true);

  void notifyHorizonChanged() noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_NET_MANAGER */
