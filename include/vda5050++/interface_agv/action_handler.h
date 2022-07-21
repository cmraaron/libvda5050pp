// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the ActionHandler declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ACTION_HANDLER
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ACTION_HANDLER

#include <optional>
#include <utility>
#include <vector>

#include "vda5050++/core/state/state_manager.h"
#include "vda5050++/model/Action.h"
#include "vda5050++/model/ErrorLevel.h"
#include "vda5050++/model/ErrorReference.h"
#include "vda5050++/model/InfoLevel.h"
#include "vda5050++/model/InfoReference.h"

namespace vda5050pp::core::logic {
class ActionManager;
}
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::interface_agv {

///
/// \brief The Interface class for action execution.
///
/// This class is meant to be specialized by the user.
/// The library will construct an object of the specialized type
/// for each individual action, such that every instance can
/// have it's own state associated with one and only one action.
///
class ActionHandler {
  friend class vda5050pp::core::logic::ActionManager;

private:
  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::interface_agv::Handle *handle_ = nullptr;

  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::core::logic::ActionManager *action_manager_ = nullptr;

  ///\brief the concerning action
  vda5050pp::Action action_;

  ///
  /// \brief Set the concerning action
  ///
  /// \param action
  ///
  void setAction(vda5050pp::Action &&action) noexcept(true);

  ///
  /// \brief Set the Handle Reference object
  /// NOTE: Needs to be set after construction
  ///
  /// \param handle the current library handle
  ///
  void setHandleReference(vda5050pp::interface_agv::Handle &handle) noexcept(true);

  ///
  /// \brief Set the Manager Reference object
  /// NOTE: Needs to be set after construction
  ///
  /// \param manager the ActionManager managing this handler
  ///
  void setManagerReference(vda5050pp::core::logic::ActionManager &manager) noexcept(true);

protected:
  ///
  /// \brief Report an error associated with this action.
  /// NOTE: error references contain action.actionId and action.actionType
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level);

  ///
  /// \brief Report an error associated with this action.
  /// NOTE: error references contain action.actionId and action.actionType
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  /// \param additional_references additional references for the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level,
                const std::vector<vda5050pp::ErrorReference> &additional_references);

  ///
  /// \brief Report an info associated with this action.
  /// NOTE: info references contain action.actionId and action.actionType
  ///
  /// \param description the description of the info
  /// \param level the level of the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level);

  ///
  /// \brief Report an info associated with this action.
  /// NOTE: info references contain action.actionId and action.actionType
  ///
  /// \param description the description of the info
  /// \param level the severity of the info
  /// \param additional_references additional references for the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level,
               const std::vector<vda5050pp::InfoReference> &additional_references);

  ///
  /// \brief Set the associated action to FAILED
  ///
  /// \throws std::logic_error if the action is not failable (not running, paused or finished)
  ///
  void failed();

  ///
  /// \brief Set the associated action to FINISHED
  ///
  /// \throws std::logic_error if the action is not finishable (not running or failed)
  ///
  void finished();

  ///
  /// \brief Set the associated action to RUNNING
  ///
  /// \throws std::logic_error if the action is not startable (not initializing)
  ///
  void started();

  ///
  /// \brief Set the associated action from RUNNING to PAUSED
  /// NOTE: this does not have to be called upon ActionHandler::pause()
  /// It has to be called whenever the associated action pauses
  ///
  /// \throws std::logic_error if the action is not pausable (not running)
  ///
  void paused();

  ///
  /// \brief Set the associated action from PAUSED to RUNNING
  /// NOTE: this does not have to be called upon ActionHandler::resume()
  /// It has to be called whenever the associated action resumes
  ///
  /// \throws std::logic_error if the action is not pausable (not running)
  ///
  void resumed();

  ///
  /// \brief Set the Result associated with the action
  ///
  /// \param result the result
  ///
  void setResult(const std::string &result);

  vda5050pp::core::state::StateManager &getState() noexcept(true);
public:
  virtual ~ActionHandler() = default;

  ///
  /// \brief The library wants the AGV to start the associated action
  ///
  /// This has to be overriden by a user instance of ActionHandler.
  /// To indicate success ActionHandler::started() has to be called.
  ///
  /// \param action the action to execute (also obtainable via ActionHandler::getAction())
  ///
  virtual void start(const vda5050pp::Action &action) = 0;

  ///
  /// \brief The library wants the AGV to pause the associated action
  ///
  /// This has to be overriden by a user instance of ActionHandler.
  /// To indicate success ActionHandler::paused() has to be called.
  ///
  /// \param action the action to execute (also obtainable via ActionHandler::getAction())
  ///
  virtual void pause(const vda5050pp::Action &action) = 0;

  ///
  /// \brief The library wants the AGV to resume the associated action.
  ///
  /// This has to be overriden by a user instance of ActionHandler.
  /// To indicate success ActionHandler::resumed() has to be called.
  ///
  /// \param action the action to execute (also obtainable via ActionHandler::getAction())
  ///
  virtual void resume(const vda5050pp::Action &action) = 0;

  ///
  /// \brief The library wants the AGV to stop an action.
  ///
  /// This has to be overriden by a user instance of ActionHandler.
  /// The action has to finish asap, either failing (ActionHandler::failed()) or finishing
  /// (ActionHandler::finished())
  ///
  /// \param action the action to execute (also obtainable via ActionHandler::getAction())
  ///
  virtual void stop(const vda5050pp::Action &action) = 0;

  ///
  /// \brief Get a reference to the associated action
  ///
  /// \return const vda5050pp::Action&
  ///
  const vda5050pp::Action &getAction() const;
};

}  // namespace vda5050pp::interface_agv

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ACTION_HANDLER */
