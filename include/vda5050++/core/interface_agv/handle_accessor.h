// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the definition of the HandleAccessor
//

#ifndef INCLUDE_VDA5050PP_CORE_INTERFACE_AGV_HANDLE_ACCESSOR_HPP_
#define INCLUDE_VDA5050PP_CORE_INTERFACE_AGV_HANDLE_ACCESSOR_HPP_

#include <cstdint>
#include <map>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "vda5050++/core/state/state.h"
#include "vda5050++/interface_agv/handle.h"
namespace vda5050pp::core::interface_agv {

///
///\brief An accessor for the handle. Used by the internal library modules to
/// find each other.
///
class HandleAccessor {
private:
  vda5050pp::interface_agv::Handle &handle_;

public:
  ///
  ///\brief Get a new HandleAccessor instance
  ///
  ///\param handle the handle to access
  ///
  explicit HandleAccessor(vda5050pp::interface_agv::Handle &handle);

  ///
  ///\brief constref for the actual handle
  ///
  ///\return const vda5050pp::interface_agv::Handle&
  ///
  const vda5050pp::interface_agv::Handle &getHandle() const noexcept(true);

  ///
  ///\brief ref for the actual handle
  ///
  ///\return vda5050pp::interface_agv::Handle&
  ///
  vda5050pp::interface_agv::Handle &getHandle() noexcept(true);

  ///
  ///\brief Constref fot the internal state of the Handle
  ///
  ///\return const vda5050pp::core::state::StateManager&
  ///
  const vda5050pp::core::state::StateManager &getState() const noexcept(true);

  ///
  ///\brief Ref fot the internal state of the Handle
  ///
  ///\return vda5050pp::core::state::StateManager&
  ///
  vda5050pp::core::state::StateManager &getState() noexcept(true);

  ///
  ///\brief Constptr for the Connector instance linked with the Handle
  ///
  ///\return const std::shared_ptr<vda5050pp::interface_mc::Connector>
  ///
  const std::shared_ptr<vda5050pp::interface_mc::Connector> getConnector() const noexcept(true);

  ///
  ///\brief ptr for the Connector instance linked with the Handle
  ///
  ///\return std::shared_ptr<vda5050pp::interface_mc::Connector>
  ///
  std::shared_ptr<vda5050pp::interface_mc::Connector> getConnector() noexcept(true);

  ///
  ///\brief Get constreference to the logic module
  ///
  ///\return const vda5050pp::core::logic::Logic&
  ///
  const vda5050pp::core::logic::Logic &getLogic() const noexcept(true);

  ///
  ///\brief Get reference to the logic module
  ///
  ///\return const vda5050pp::core::logic::Logic&
  ///
  vda5050pp::core::logic::Logic &getLogic() noexcept(true);

  ///
  ///\brief Get constreference to the validation module
  ///
  ///\return vda5050pp::core::validation::ValidationProvider&
  ///
  const vda5050pp::core::validation::ValidationProvider &getValidationProvider() const
      noexcept(true);

  ///
  ///\brief Get reference to the validation module
  ///
  ///\return vda5050pp::core::validation::ValidationProvider&
  ///
  vda5050pp::core::validation::ValidationProvider &getValidationProvider() noexcept(true);

  ///
  ///\brief Get constref to the message module
  ///
  ///\return const vda5050pp::core::messages::Messages&
  ///
  const vda5050pp::core::messages::Messages &getMessages() const noexcept(true);

  ///
  ///\brief Get reference to the message module
  ///
  ///\return const vda5050pp::core::messages::Messages&
  ///
  vda5050pp::core::messages::Messages &getMessages() noexcept(true);

  ///
  ///\brief Return a valid logger for the Handle. Either the configured handle, or
  /// a logger stub, discarding all values
  ///
  ///\return vda5050pp::interface_agv::Logger&
  ///
  vda5050pp::interface_agv::Logger &getLogger() const noexcept(true);

  ///
  ///\brief Create a new ActionHandler
  ///
  ///\return std::shared_ptr<vda5050pp::interface_agv::ActionHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::ActionHandler> createActionHandler() const
      noexcept(true);

  ///
  ///\brief Create a new StepBasedNavigationHandler
  ///
  ///\return std::shared_ptr<vda5050pp::interface_agv::StepBasedNavigationHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::StepBasedNavigationHandler>
  createStepBasedNavigationHandler() const noexcept(true);

  ///
  ///\brief Create a new PauseResumeHandler
  ///
  ///\return std::shared_ptr<vda5050pp::interface_agv::PauseResumeHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::PauseResumeHandler> createPauseResumeHandler() const
      noexcept(true);

  ///
  ///\brief Create a new ContinuousNavigationHandler
  ///
  ///\return std::shared_ptr<vda5050pp::interface_agv::ContinuousNavigationHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::ContinuousNavigationHandler>
  createContinuousNavigationHandler() const noexcept(true);

  ///
  ///\brief returns true, iff a ContinuousNavigationHandler was set
  ///
  ///\return true/false
  ///
  bool isContinuousNavigation() const noexcept(true);

  ///
  ///\brief returns true, iff a StepBaseNavigationHandler was set
  ///
  ///\return true/false
  ///
  bool isStepBasedNavigation() const noexcept(true);

  vda5050pp::core::common::BlockingQueue<std::function<void()>> &getTaskQueue() noexcept(true);

  const vda5050pp::interface_agv::agv_description::AGVDescription &getAGVDescription() const
      noexcept(true);

  ///
  ///\brief get the stats update period
  ///
  ///\return std::chrono::system_clock::duration the state update period
  ///
  std::chrono::system_clock::duration getStateUpdatePeriod() const noexcept(true);

  ///
  ///\brief Get a shared_ptr to the currently set OdometryHandler
  ///
  ///\return std::shared_ptr<vda5050pp::interface_agv::OdometryHandler>
  ///
  std::shared_ptr<vda5050pp::interface_agv::OdometryHandler> getOdometryHandler() const
      noexcept(true);
};

}  // namespace vda5050pp::core::interface_agv

#endif  // INCLUDE_VDA5050PP_CORE_INTERFACE_AGV_HANDLE_ACCESSOR_HPP_
