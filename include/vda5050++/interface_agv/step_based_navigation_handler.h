// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STEP_BASED_NAVIGATION_HANDLER
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STEP_BASED_NAVIGATION_HANDLER

#include <optional>
#include <vector>

#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/Edge.h"
#include "vda5050++/model/ErrorLevel.h"
#include "vda5050++/model/ErrorReference.h"
#include "vda5050++/model/InfoLevel.h"
#include "vda5050++/model/InfoReference.h"
#include "vda5050++/model/Node.h"

namespace vda5050pp::core::logic {
class DriveToNodeManager;
}
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::interface_agv {

///
/// \brief Navigation Handler base for "line-guided" behaviour.
///
/// This class can be specialized by the user to implement "line-guided" behaviour.
/// For each driving step an object of the specialized class will be created, such that
/// each instance can maintain a state for one and only one driving goal.
///
class StepBasedNavigationHandler {
  friend class vda5050pp::core::logic::DriveToNodeManager;

private:
  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::interface_agv::Handle *handle_ = nullptr;

  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::core::logic::DriveToNodeManager *manager_ = nullptr;

  ///\brief the associated goal node
  vda5050pp::Node goal_node_;

  ///\brief the associated via edge
  std::optional<vda5050pp::Edge> via_edge_;

  ///
  /// \brief set the associated data for this handler
  ///
  /// \param goal_node the goal node
  /// \param via_edge  the via edge
  ///
  void setData(vda5050pp::Node &&goal_node,
               std::optional<vda5050pp::Edge> &&via_edge) noexcept(true);

  ///
  /// \brief Set the Manager Reference object
  /// NOTE: Needs to be set after construction
  ///
  /// \param manager the DriveToNodeManager managing this handler
  ///
  void setManager(vda5050pp::core::logic::DriveToNodeManager &manager) noexcept(true);

  ///
  /// \brief Set the Handle Reference object
  /// NOTE: Needs to be set after construction
  ///
  /// \param handle the current library handle
  ///
  void setHandle(vda5050pp::interface_agv::Handle &handle) noexcept(true);

protected:
  ///
  /// \brief Report an error associated this navigation goal.
  /// NOTE: error reference contains goal_node.sequenceId
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level);

  ///
  /// \brief Report an error associated this navigation goal.
  /// NOTE: error reference contains goal_node.sequenceId
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  /// \param additional_references additional references for the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level,
                const std::vector<vda5050pp::ErrorReference> &additional_references);

  ///
  /// \brief Report an error associated this navigation goal.
  /// NOTE: error reference contains goal_node.sequenceId
  ///
  /// \param description the description of the info
  /// \param level the level of the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level);

  ///
  /// \brief Report an error associated this navigation goal.
  /// NOTE: error reference contains goal_node.sequenceId
  ///
  /// \param description the description of the info
  /// \param level the severity of the info
  /// \param additional_references additional references for the info
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
  /// \brief Set the associated navigation to FINISHED
  ///
  /// \throws std::logic_error if navigation is not finishable (not running or failed)
  ///
  void finished();

  ///
  /// \brief Set the associated navigation to RUNNING
  ///
  /// \throws std::logic_error if navigation is not startable (not initializing)
  ///
  void started();

  ///
  /// \brief Set the associated navigation from RUNNING to PAUSED
  /// NOTE: This does not have to be called only upon StepBasedNavigationHandler::pause()
  /// This must be called everytime driving pauses.
  ///
  /// \throws std::logic_error if navigation is not pausable (not running)
  ///
  void paused();

  ///
  /// \brief Set the associated navigation from PAUSED to RUNNING
  /// NOTE: This does not have to be called only upon StepBasedNavigationHandler::resume()
  /// This must be called everytime driving resumes.
  ///
  /// \throws std::logic_error if navigation is not pausable (not running)
  ///
  void resumed();

  ///
  /// \brief Set the current position of the AGV (optional)
  ///
  /// \param position the current position
  ///
  void setPosition(const vda5050pp::AGVPosition &position);

  ///
  /// \brief update the distance driven since the last node (optional)
  ///
  /// \param distance [m] the distance driven since the last node
  ///
  void updateDistanceSinceLastNode(double distance);

public:
  virtual ~StepBasedNavigationHandler() = default;

  ///
  /// \brief The library wants the AGV to start navigation to the goal node via an edge
  ///
  /// This has to be overriden by a user instance of StepBasedNavigationHandler.
  /// To indicate success ActionHandler::started() has to be called.
  ///
  /// \param via_edge the edge leading to goal_node
  /// \param goal_node  the goal node
  ///
  virtual void start(const std::optional<vda5050pp::Edge> &via_edge,
                     const vda5050pp::Node &goal_node) = 0;

  ///
  /// \brief The library wants the AGV to pause navigation
  ///
  /// This has to be overriden by a user instance of StepBasedNavigationHandler.
  /// To indicate success ActionHandler::paused() has to be called.
  ///
  virtual void pause() = 0;

  ///
  /// \brief The library wants the AGV to resume navigation
  ///
  /// This has to be overriden by a user instance of StepBasedNavigationHandler.
  /// To indicate success ActionHandler::resumed() has to be called.
  ///
  virtual void resume() = 0;

  ///
  /// \brief The library wants the AGV to stop navigation
  ///
  /// This has to be overriden by a user instance of StepBasedNavigationHandler.
  /// The AGV has to stop asap, stopping between nodes if possible
  /// (StepBasedNavigationHandler::failed()), otherwise finishing this navigation step
  /// (StepBasedNavigationHandler::finished()).
  ///
  virtual void stop() = 0;

  ///
  /// \brief Get the Goal Node
  ///
  /// \return const vda5050pp::Node&
  ///
  const vda5050pp::Node &getGoalNode() const;

  ///
  /// \brief Get the Via Edge
  ///
  /// \return const vda5050pp::Edge&
  ///
  const std::optional<vda5050pp::Edge> &getViaEdge() const;
};

}  // namespace vda5050pp::interface_agv

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_STEP_BASED_NAVIGATION_HANDLER */
