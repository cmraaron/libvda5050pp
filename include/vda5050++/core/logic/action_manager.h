// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the ActionManager declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ACTION_MANAGER
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ACTION_MANAGER

#include <memory>

#include "vda5050++/core/logic/task_manager.h"
#include "vda5050++/core/logic/types.h"
#include "vda5050++/interface_agv/action_handler.h"
#include "vda5050++/model/Action.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
/// \brief Specialization of the TaskManager for Action tasks
///
class ActionManager : public TaskManager {
private:
  std::shared_ptr<vda5050pp::interface_agv::ActionHandler> action_handler_;

  virtual bool logTransition(const std::string &name, bool ret) noexcept(true) override;
  virtual void logPlaceReached(const Net::PlaceT &place) noexcept(true) override;

  virtual void taskInitialize() noexcept(true) override;
  virtual void taskRunning() noexcept(true) override;
  virtual void taskPaused() noexcept(true) override;
  virtual void taskFinished() noexcept(true) override;
  virtual void taskFailed() noexcept(true) override;

public:
  ///
  ///\brief Create a new ActionManager which owns an ActionHandle and can be attached to the
  /// globale PTN
  ///
  ///\param handle the library handle
  ///\param action the associated action
  ///\param seq the sequence id inside of the net
  ///
  ActionManager(vda5050pp::interface_agv::Handle &handle, vda5050pp::Action action, SeqNrT seq);

  ///
  /// \brief Get a pointer to the associated ActionHandler
  ///
  /// \return std::shared_ptr<vda5050pp::interface_agv::ActionHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::ActionHandler> getHandler() const noexcept(true);

  ///
  ///\brief Try to pause the associated task.
  ///
  virtual void pause() noexcept(true) override;

  ///
  ///\brief Try to resume the associated task.
  ///
  virtual void resume() noexcept(true) override;

  ///
  ///\brief Stop the associated action
  ///
  virtual void stop() noexcept(true) override;
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_ACTION_MANAGER */
