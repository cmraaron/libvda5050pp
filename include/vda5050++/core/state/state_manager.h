// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the StateManager
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE_MANAGER
#define INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE_MANAGER

#include <functional>
#include <list>

#include "vda5050++/core/state/state.h"
#include "vda5050++/model/InstantActions.h"
#include "vda5050++/model/Order.h"

namespace vda5050pp::core::state {

///
/// \brief This class manages the vda5050 state of the library
///
class StateManager {
private:
  State state_;

  std::string getGraphIdBySeqIdAcquired(uint32_t seq_id) const noexcept(false);

public:
  StateManager() noexcept(true);

  ///
  /// \brief Get the raw state (avoid this unless it is strictly required)
  ///
  /// \return State&
  ///
  State &getStateUnsafe() noexcept(true);

  ///
  /// \brief Get an action by it's id
  ///
  /// \param id the action's id
  /// \throws std::invalid_argument on unknown id
  /// \return vda5050pp::Action
  ///
  vda5050pp::Action getActionById(const std::string &id) const noexcept(false);

  ///
  /// \brief Set the ActionStatus of an action
  ///
  /// \param id the action's id
  /// \param status the status
  /// \throws std::invalid_argument on unknown id
  ///
  void setActionStatus(const std::string &id, vda5050pp::ActionStatus status) noexcept(false);

  ///
  /// \brief Set the ActionResult of an action
  ///
  /// \param id the action's id
  /// \param result the result
  /// \throws std::invalid_argument on unknown id
  ///
  void setActionResult(const std::string &id, const std::string &result) noexcept(false);

  ///
  /// \brief Unset the ActionResult of an action
  ///
  /// \param id the action's id
  /// \throws std::invalid_argument on unknown id
  ///
  void unsetActionResult(const std::string &id) noexcept(false);

  ///
  /// \brief Get the id of an graph element by it's (vda5050) sequence id
  ///
  /// \param seq_id the sequence id
  /// \throws std::invalid_argument on unknown id
  /// \return std::string the element's id
  ///
  std::string getGraphIdBySeqId(uint32_t seq_id) const noexcept(false);

  ///
  /// \brief Append an order to the state
  ///
  /// This extends the current order.
  /// NOTE: The Order MUST be validated before appending it, otherwise it results in undefined
  /// behavior
  ///
  /// \param order the order to append
  ///
  void appendOrder(const vda5050pp::Order &order) noexcept(true);

  ///
  /// \brief Overwrite the current state with a fresh order
  ///
  /// \param order the new order
  ///
  void setOrder(const vda5050pp::Order &order) noexcept(true);

  ///
  /// \brief Clear the current order (and all instant actions)
  ///
  void clearOrder() noexcept(true);

  ///
  /// \brief Insert instant actions into the state
  ///
  /// \param instant_actions the new instant actions
  ///
  void insertInstantActions(const vda5050pp::InstantActions &instant_actions) noexcept(true);

  ///
  /// \brief Set the current zoneSetId
  ///
  /// \param id the zoneSetId
  ///
  void setZoneSetId(const std::string &id) noexcept(true);

  ///
  /// \brief Unset the current zoneSetId
  ///
  void unsetZoneSetId() noexcept(true);

  ///
  ///\brief Set the last reached node, not revertible because
  /// all previous node and edge states will be cleared to indicate
  /// progress. (also sets lastNodeId and lastNodeSequenceId)
  ///
  ///\param node the node reached
  ///
  void setLastNodeReached(uint32_t seq_id) noexcept(false);

  ///
  /// \brief set only the lastNodeId field in the state
  ///
  /// \param node_id the last node id
  ///
  void setLastNode(const std::string &node_id) noexcept(true);

  ///
  /// \brief Set the AGVPosition
  ///
  /// \param position position
  ///
  void setAGVPosition(const vda5050pp::AGVPosition &position) noexcept(true);

  ///
  /// \brief Unset the AGVPosition
  ///
  void unsetAGVPosition() noexcept(true);

  ///
  /// \brief Set the Velocity
  ///
  /// \param velocity velocity
  ///
  void setVelocity(const vda5050pp::Velocity &velocity) noexcept(true);

  ///
  /// \brief Unset the Velocity
  ///
  void unsetVelocity() noexcept(true);

  ///
  /// \brief Add a load (enables state.loads)
  ///
  /// \param load load
  ///
  void addLoad(const vda5050pp::Load &load) noexcept(true);

  ///
  /// \brief Remove a load with a certain id (enables state.loads)
  ///
  /// \param load_id the id of the load to remove
  ///
  void removeLoad(const std::string &load_id) noexcept(true);

  ///
  /// \brief The exact load object to remove (enables state.loads)
  ///
  /// \param load the load to remove
  ///
  void removeLoad(const vda5050pp::Load &load) noexcept(true);

  ///
  /// \brief Get the Load by it's id
  ///
  /// \param load_id the load's id
  /// \throws std::invalid_argument on unknown id
  /// \return vda5050pp::Load
  ///
  vda5050pp::Load getLoad(const std::string &load_id) const noexcept(false);

  ///
  /// \brief Get all current Loads
  ///
  /// \return std::vector<vda5050pp::Load>
  ///
  std::optional<std::vector<vda5050pp::Load>> getLoads() const noexcept(true);

  ///
  ///\brief Unset the loads field from the vda5050 state.
  ///
  void unsetLoads() noexcept(true);

  ///
  /// \brief Set the Driving status
  ///
  /// \param driving is driving?
  ///
  void setDriving(bool driving) noexcept(true);

  ///
  /// \brief Get the Driving status
  ///
  /// \return isDriving
  ///
  bool isDriving() const noexcept(true);

  ///
  /// \brief Set the requestNewBase field
  ///
  void requestNewBase() noexcept(true);

  ///
  ///\brief Checks if there are no node and edge states left
  ///
  ///\return is idle?
  ///
  bool isIdle() const noexcept(true);

  ///
  /// \brief Set the distance since last node
  ///
  /// \param distance
  ///
  void setDistanceSinceLastNode(double distance) noexcept(true);

  ///
  /// \brief Unset the distance since last node
  ///
  ///
  void unsetDistanceSinceLastNode() noexcept(true);

  ///
  /// \brief Get the current BatteryState
  ///
  /// \return vda5050pp::BatteryState
  ///
  vda5050pp::BatteryState getBatteryState() const noexcept(true);

  ///
  /// \brief Set the current BatteryState
  ///
  /// \param battery_state BatteryState
  ///
  void setBatteryState(const vda5050pp::BatteryState &battery_state) noexcept(true);

  ///
  /// \brief Get the current SafetyState
  ///
  /// \return vda5050pp::SafetyState
  ///
  vda5050pp::SafetyState getSafetyState() const noexcept(true);

  ///
  /// \brief Set the current SafetyState
  ///
  /// \param safety_state
  ///
  void setSafetyState(const vda5050pp::SafetyState &safety_state) noexcept(true);

  ///
  /// \brief Add an Error
  ///
  /// \param error the error
  ///
  void addError(const vda5050pp::Error &error) noexcept(true);

  ///
  /// \brief Remove an error matching a predicate
  ///
  /// \param pred the predicate
  /// \return number of removed errors
  ///
  size_t removeError(const std::function<bool(const vda5050pp::Error &)> &pred) noexcept(true);

  ///
  /// \brief Ensures a certain error is set
  ///
  /// \param error the error to ensure
  /// \return if error was set
  ///
  bool ensureError(const vda5050pp::Error &error) noexcept(true);

  ///
  /// \brief Add an Info
  ///
  /// \param info the info
  ///
  void addInfo(const vda5050pp::Info &info) noexcept(true);

  ///
  /// \brief Remove an info matching a predicate
  ///
  /// \param pred the predicate
  /// \return number of removed infos
  ///
  size_t removeInfo(const std::function<bool(const vda5050pp::Info &)> &pred) noexcept(true);

  ///
  /// \brief Get the next seq for the state topic
  ///
  /// \return uint32_t sequence number
  ///
  uint32_t nextStateSeq() noexcept(true);

  ///
  /// \brief Get the next seq for the visualization topic
  ///
  /// \return uint32_t sequence number
  ///
  uint32_t nextVisualizationSeq() noexcept(true);

  ///
  /// \brief Get the next seq for the connection topic
  ///
  /// \return uint32_t sequence number
  ///
  uint32_t nextConnectionSeq() noexcept(true);

  ///
  /// \brief Dump the current State as a pure vda5050 state
  /// NOTE: Only the header is unset
  ///
  /// \return vda5050pp::State the pure state
  ///
  vda5050pp::State dumpState() const noexcept(true);

  ///
  /// \brief Does the seqId belong to a node?
  ///
  /// \param seq
  /// \return answer
  ///
  static bool isNode(uint32_t seq) noexcept(true);

  ///
  /// \brief Does the seqId belong to an edge?
  ///
  /// \param seq
  /// \return answer
  ///
  static bool isEdge(uint32_t seq) noexcept(true);

  ///
  /// \brief Get the Node with a certain sequenceId
  ///
  /// \param seq
  /// \throws std::invalid_argument is seq is not associated
  /// \return vda5050pp::Node
  ///
  vda5050pp::Node getNodeBySeq(uint32_t seq) const noexcept(false);

  ///
  /// \brief Get the Edge with a certain sequenceId
  ///
  /// \param seq
  /// \throws std::invalid_argument is seq is not associated
  /// \return vda5050pp::Edge
  ///
  vda5050pp::Edge getEdgeBySeq(uint32_t seq) const noexcept(false);

  ///
  /// \brief Get the last sequenceId of the base
  ///
  /// \return uint32_t
  ///
  uint32_t getGraphBaseSeqId() const noexcept(true);

  ///
  /// \brief Get the last sequenceId of the horizon
  ///
  /// \return uint32_t
  ///
  uint32_t getGraphHorizonSeqId() const noexcept(true);

  ///
  /// \brief Get the sequenceId of the element the AGV is currently at
  ///
  /// \return uint32_t
  ///
  uint32_t getAGVSequenceId() const noexcept(true);

  ///
  /// \brief Get the OrderId
  ///
  /// \return const std::string&
  ///
  const std::string &getOrderId() const noexcept(true);

  ///
  /// \brief Get the OrderUpdateId
  ///
  /// \return uint32_t
  ///
  uint32_t getOrderUpdateId() const noexcept(true);

  ///
  /// \brief Get the PausedState
  ///
  /// \return pausedState
  ///
  bool getPausedState() const noexcept(true);

  ///
  /// \brief Set the PausedState
  ///
  /// \param paused
  ///
  void setPausedState(bool paused) noexcept(true);

  ///
  /// \brief Get the current OperatingMode
  ///
  /// \return std::optional<vda5050pp::OperatingMode>
  ///
  vda5050pp::OperatingMode getOperatingMode() const noexcept(true);

  ///
  /// \brief Set the current OperatingMode
  ///
  /// \param operating_mode
  ///
  void setOperatingMode(const vda5050pp::OperatingMode &operating_mode) noexcept(true);

  ///
  /// \brief Get the next node (may not be present)
  ///
  /// \return std::optional<std::reference_wrapper<vda5050pp::Node>>
  ///
  std::optional<vda5050pp::Node> getNextNode() const noexcept(true);

  ///
  ///\brief Get the current known base nodes (does not include traversed nodes)
  ///
  ///\return std::list<vda5050pp::Node>
  ///
  std::list<vda5050pp::Node> getBaseNodes() const noexcept(true);

  ///
  ///\brief Get the currently known base edges (does not include traversed edges)
  ///
  ///\return std::list<vda5050pp::Edge>
  ///
  std::list<vda5050pp::Edge> getBaseEdges() const noexcept(true);

  ///
  ///\brief Get the currently known horizon nodes
  ///
  ///\return std::list<vda5050pp::Node>
  ///
  std::list<vda5050pp::Node> getHorizonNodes() const noexcept(true);

  ///
  ///\brief Get the currently known horizon edges
  ///
  ///\return std::list<vda5050pp::Edge>
  ///
  std::list<vda5050pp::Edge> getHorizonEdges() const noexcept(true);

  ///
  /// \brief Get the current AGV position
  ///
  /// \return vda5050pp::AGVPosition
  ///
  std::optional<vda5050pp::AGVPosition> getAGVPosition() const noexcept(true);

  ///
  /// \brief Get the current velocity
  ///
  /// \return std::optional<vda5050pp::Velocity>
  ///
  std::optional<vda5050pp::Velocity> getVelocity() const noexcept(true);

  ///
  /// \brief Get the Last nodeId
  ///
  /// \return std::string
  ///
  std::string getLastNodeId() const noexcept(true);

  ///
  /// \brief Remove the action state of an action
  ///
  /// \param actionId the actionId of the Action
  ///
  /// \throws std::invalid_argument when actionId does not exist
  ///
  void removeActionState(const std::string &actionId) noexcept(false);
};

}  // namespace vda5050pp::core::state

#endif /* INCLUDE_VDA5050_2B_2B_CORE_STATE_STATE_MANAGER */
