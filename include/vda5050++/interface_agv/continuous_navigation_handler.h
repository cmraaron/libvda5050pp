// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the declaration of the ContinuousNavigationHandler
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_CONTINUOUS_NAVIGATION_HANDLER
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_CONTINUOUS_NAVIGATION_HANDLER

#include <list>
#include <vector>

#include "vda5050++/core/common/math/linear_path_length_calculator.h"
#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/Edge.h"
#include "vda5050++/model/ErrorLevel.h"
#include "vda5050++/model/ErrorReference.h"
#include "vda5050++/model/InfoLevel.h"
#include "vda5050++/model/InfoReference.h"
#include "vda5050++/model/Node.h"

/// Forward declaration to avoid cyclic dependencies
namespace vda5050pp::core::logic {
class ContinuousNavigationManager;
}

namespace vda5050pp::interface_agv {
/// Forward declaration to avoid cyclic dependencies
class Handle;

///
/// \brief Navigation Handler base for "continuous navigation" behaviour.
///
/// This class can be specialized by the user to implement a continuous driving behaviour.
/// For each connected path (not interrupted by SOFT or HARD action). One specialized
/// ContinuousNavigationHandler is created. When the order is still beeing extended
/// the currently active ContinuousNaviationHandler will be notified about the new
/// subgraph.
///
class ContinuousNavigationHandler {
  friend class vda5050pp::core::logic::ContinuousNavigationManager;

private:
  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::interface_agv::Handle *handle_ = nullptr;

  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::core::logic::ContinuousNavigationManager *manager_ = nullptr;

  ///\brief current base
  std::list<vda5050pp::Node> base_nodes_;
  ///\brief current horizon
  std::list<vda5050pp::Node> horz_nodes_;
  ///\brief current base
  std::list<vda5050pp::Edge> base_edges_;
  ///\brief current horizon
  std::list<vda5050pp::Edge> horz_edges_;
  ///\brief end of the old base (updated in appendToBase call)
  std::list<vda5050pp::Node>::iterator base_old_last_node_;
  ///\brief end of the old base (updated in appendToBase call)
  std::list<vda5050pp::Edge>::iterator base_old_last_edge_;

  vda5050pp::core::common::math::LinearPathLengthCalculator<double> path_length_calculator_;

  ///
  /// \brief Set the Handle Reference object (must be called before start())
  /// NOTE: Needs to be set after construction
  ///
  /// \param handle the current library handle
  ///
  void setHandle(vda5050pp::interface_agv::Handle &handle) noexcept(true);

  ///
  /// \brief Set the manager (must be called before start())
  ///
  /// \param mgr  the manager
  ///
  void setManager(vda5050pp::core::logic::ContinuousNavigationManager &mgr) noexcept(true);

  ///
  /// \brief Set the Initial Base
  ///
  /// \param initial_base_nodes all base nodes
  /// \param initial_base_edges  all base edges
  ///
  void setInitialBase(std::list<vda5050pp::Node> &&initial_base_nodes,
                      std::list<vda5050pp::Edge> &&initial_base_edges);

  ///
  /// \brief Overwrite the current horizon
  ///
  /// \param new_horizon_nodes the new horizon nodes
  /// \param new_horizon_edges the new horizon edges
  ///
  void setNewHorizon(std::list<vda5050pp::Node> &&new_horizon_nodes,
                     std::list<vda5050pp::Edge> &&new_horizon_edges);

  ///
  /// \brief Append to the current base
  ///
  /// \param new_base_nodes the base nodes to append
  /// \param new_base_edges the base edges to append
  ///
  void appendToBase(std::list<vda5050pp::Node> &&new_base_nodes,
                    std::list<vda5050pp::Edge> &&new_base_edges);

protected:
  ///
  /// \brief Get the last reached node
  ///
  /// \return const vda5050pp::Node&
  ///
  const vda5050pp::Node &getLastNode() const;

  ///
  /// \brief Get the next node to navigate to (might be non existing)
  ///
  /// \return std::optional<std::reference_wrapper<const vda5050pp::Node>>
  ///
  std::optional<std::reference_wrapper<const vda5050pp::Node>> getNextNode() const;

  ///
  /// \brief Get the edge leading to the next node (might be non existing)
  ///
  /// \return std::optional<std::reference_wrapper<const vda5050pp::Edge>>
  ///
  std::optional<std::reference_wrapper<const vda5050pp::Edge>> getNextEdge() const;

  ///
  /// \brief Get the current base nodes
  ///
  /// NOTE: The base does not contain traversed nodes
  ///
  /// \return const std::list<vda5050pp::Node>&
  ///
  const std::list<vda5050pp::Node> &getBaseNodes() const;

  ///
  /// \brief Get the current base edges
  ///
  /// NOTE: The base does not contain traversed edges
  ///
  /// \return const std::list<vda5050pp::Node>&
  ///
  const std::list<vda5050pp::Edge> &getBaseEdges() const;

  ///
  /// \brief Get the current horizon nodes (these must not be traversed, until they become part of
  /// the base)
  ///
  /// \return const std::list<vda5050pp::Node>&
  ///
  const std::list<vda5050pp::Node> &getHorizonNodes() const;

  ///
  /// \brief Get the current horizon edges (these must not be traversed, until they become part of
  /// the base)
  ///
  /// \return const std::list<vda5050pp::Node>&
  ///
  const std::list<vda5050pp::Edge> &getHorizonEdges() const;

  ///
  /// \brief When the base was increased this yields the nodes that were appended since the last
  /// baseIncreased() call (including the last base node)
  ///
  /// NOTE: Will be reset once a new node is reached
  ///
  /// \return std::list<vda5050pp::Node>
  ///
  std::list<vda5050pp::Node> getBaseDeltaNodes() const;

  ///
  /// \brief When the base was increased this yields the edges that were appended since the last
  /// baseIncreased() call
  ///
  /// NOTE: Will be reset once a new node is reached
  ///
  /// \return std::list<vda5050pp::Node>
  ///
  std::list<vda5050pp::Edge> getBaseDeltaEdges() const;

  ///
  /// \brief Add an error referencing the current navigation goal
  ///
  /// \param description error description
  /// \param level the severity of the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level);
  ///
  /// \brief Add an error referencing the current navigation goal
  ///
  /// \param description error description
  /// \param level the severity of the error
  /// \param additional_references additional references for the error (goal_node.sequenceId will be
  /// set automatically)
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level,
                const std::vector<vda5050pp::ErrorReference> &additional_references);
  ///
  /// \brief Add an information referencing the current navigation goal
  ///
  /// \param description info description
  /// \param level the level of the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level);

  ///
  /// \brief Add an information referencing the current navigation goal
  ///
  /// \param description info description
  /// \param level the level of the info
  /// \param additional_references additional references for the info (goal_node.sequenceId will be
  /// set automatically)
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level,
               const std::vector<vda5050pp::InfoReference> &additional_references);

  ///
  /// \brief Set the associated navigation to FAILED
  ///
  /// \throws std::logic_error if navigation is not failable (not running, paused or finished)
  ///
  void failed();

  ///
  /// \brief Set the associated navigation to RUNNING
  ///
  /// \throws std::logic_error if navigation is not startable (not initializing)
  ///
  void started();

  ///
  /// \brief Set the associated navigation from RUNNING to PAUSED
  /// NOTE: This does not have to be called only upon ContinuousNavigationHandler::pause()
  /// This must be called everytime driving pauses.
  ///
  /// \throws std::logic_error if navigation is not pausable (not running)
  ///
  void paused();

  ///
  /// \brief Set the associated navigation from PAUSED to RUNNING
  /// NOTE: This does not have to be called only upon ContinuousNavigationHandler::resume()
  /// This must be called everytime driving resumes.
  ///
  /// \throws std::logic_error if navigation is not pausable (not running)
  ///
  void resumed();

  ///
  /// \brief Tell the library, that a specific node was reached (alternative to evalPosition).
  /// Same as setNodeReached(uint32_t), this just extracts the id.
  ///
  /// NOTE: cannot be undone (set to predecessor node)
  ///
  /// \param node the node reached
  ///
  void setNodeReached(const vda5050pp::Node &node) noexcept(false);

  ///
  /// \brief Tell the library, that a specific node was reached (alternative to evalPosition)
  /// Same as setNodeReached(const vda5050pp::Node &), this does not extract the id.
  ///
  /// NOTE: cannot be undone (set to predecessor node)
  ///
  /// \param node_seq the sequence id of the node
  ///
  void setNodeReached(uint32_t node_seq) noexcept(false);

  ///
  /// \brief Set the current position (full vda5050 data) and check if the next node was reached to
  /// indicate task progress (alternative to setNodeReached).
  ///
  /// \param position the position of the AGV
  /// \return true, if the next node was reached
  ///
  bool evalPosition(const vda5050pp::AGVPosition &position) noexcept(false);

  ///
  /// \brief Set the current position of the AGV and check if the next node was reached to
  /// indicate task progress (alternative to setNodeReached).
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \return true, if the next node was reached
  ///
  bool evalPosition(double x, double y, double theta, const std::string &map) noexcept(false);

  ///
  /// \brief Set the current position with deviation of the AGV and check if the next node was
  /// reached to indicate task progress (alternative to setNodeReached).
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \param deviation deviation of the position (if the node deviation fully encloses the agv
  /// deviation, it is considered as reached)
  /// \return true, if the next node was reached
  ///
  bool evalPosition(double x, double y, double theta, const std::string &map,
                    double deviation) noexcept(false);

  ///
  /// \brief Set the current position (full vda5050 data). Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param position the position of the AGV
  ///
  void setPosition(const vda5050pp::AGVPosition &position) noexcept(false);

  ///
  /// \brief Set the current position of the AGV. Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta
  ///
  void setPosition(double x, double y, double theta, const std::string &map) noexcept(false);

  ///
  /// \brief Set the current position with deviation of the AGV. Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \param deviation deviation of the position (if the nodes deviation and the agvs deviation
  /// intersect, the node is reached)
  ///
  void setPosition(double x, double y, double theta, const std::string &map,
                   double deviation) noexcept(false);

public:
  virtual ~ContinuousNavigationHandler() = default;

  ///
  /// \brief The horizon was updated. This can be used to prepare the AGV for upcoming nodes.
  /// NOTE: The horizon must not be traversed until it becomes the base.
  ///
  /// \param new_horizon_nodes the new horizon nodes
  /// \param new_horizon_edges the new horizon edges
  ///
  virtual void horizonUpdated(const std::list<vda5050pp::Node> &new_horizon_nodes,
                              const std::list<vda5050pp::Edge> &new_horizon_edges) = 0;

  ///
  /// \brief The base has increased. The AGV can now traverse the new graph passed as parameter.
  ///
  /// \param base_append_nodes the new base nodes
  /// \param base_append_edges the new base edges
  ///
  virtual void baseIncreased(const std::list<vda5050pp::Node> &base_append_nodes,
                             const std::list<vda5050pp::Edge> &base_append_edges) = 0;

  ///
  /// \brief Start a new driving path. A path is segmented into parts which are not
  /// separated by a blocking action. The library calls start() only once per handler.
  /// When a new path is constructed, then a new handler is created.
  ///
  /// \param base_nodes The initial base nodes to traverse.
  /// \param base_edges The initial base edges to use (may be empty if the first node of the graph
  /// is "trivially" reachable and the base has only one node).
  ///
  virtual void start(const std::list<vda5050pp::Node> &base_nodes,
                     const std::list<vda5050pp::Edge> &base_edges) = 0;

  ///
  /// \brief The AGV has to pause navigation. Call paused() to indicate a successful pause.
  ///
  virtual void pause() = 0;

  ///
  /// \brief The AGV has to resume navigation. Call resumed() to indicate a successful resume.
  ///
  virtual void resume() = 0;

  ///
  /// \brief The AGV has to stop navigation asap.
  /// Either stopping in place or at the next node, if it cannot continue navigation in
  /// between nodes.
  ///
  /// A stop in between nodes must be indicated via ContinuousNavigationHandler::failed(), because
  /// the associated navigation step (to the current node) was not successful.
  /// A stop at the next node will simply be reported by
  /// ContinuousNavigationHander::setNodeReached() or ContinuousNavigationHander::evalPosition().
  /// Once the current node goal was reached, it is not possible to reach the next node.
  ///
  virtual void stop() = 0;
};

}  // namespace vda5050pp::interface_agv

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_CONTINUOUS_NAVIGATION_HANDLER */
