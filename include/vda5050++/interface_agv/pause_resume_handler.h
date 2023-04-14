// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_PAUSE_RESUME_HANDLER
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_PAUSE_RESUME_HANDLER

#include <optional>
#include <utility>
#include <vector>

#include "vda5050++/model/ErrorLevel.h"
#include "vda5050++/model/ErrorReference.h"
#include "vda5050++/model/InfoLevel.h"
#include "vda5050++/model/InfoReference.h"

namespace vda5050pp::core::logic {
class PauseResumeActionManager;
}
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::interface_agv {

///
/// \brief The interface class for a pause or resume request
///
/// The user can specialize this class to provide an endpoint for pause or resume requests.
/// The purpose of this interface is to provide an interface for the library to check if pausing
/// or resuming is possible.
///
class PauseResumeHandler {
  friend class vda5050pp::core::logic::PauseResumeActionManager;

private:
  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::interface_agv::Handle *handle_ = nullptr;

  ///\brief To avoid passing this via the constructor (and the interface)
  ///       the pointer is set later. (Sadly std::reference_wrapper does not support incomplete
  ///       types)
  vda5050pp::core::logic::PauseResumeActionManager *manager_ = nullptr;

  ///\brief indicates pause or resume
  bool is_pause_;

  ///\brief set pause or resume (should be called immediately after construction)
  void setPauseOrResume(bool pause) noexcept(true);

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
  /// \param manager the PauseResumeActionManager managing this handler
  ///
  void setManagerReference(vda5050pp::core::logic::PauseResumeActionManager &manager) noexcept(
      true);

protected:
  ///
  /// \brief Report an error associated with this request.
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level);

  ///
  /// \brief Report an error associated with this request.
  ///
  /// \param description the description of the error
  /// \param level the severity of the error
  /// \param additional_references additional references for the error
  ///
  void addError(const std::string &description, vda5050pp::ErrorLevel level,
                const std::vector<vda5050pp::ErrorReference> &additional_references);

  ///
  /// \brief Report an info associated with this request.
  ///
  /// \param description the description of the info
  /// \param level the level of the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level);

  ///
  /// \brief Report an info associated with this request.
  ///
  /// \param description the description of the info
  /// \param level the severity of the info
  /// \param additional_references additional references for the info
  ///
  void addInfo(const std::string &description, vda5050pp::InfoLevel level,
               const std::vector<vda5050pp::InfoReference> &additional_references);

  ///
  /// \brief The AGV is not pausable/resumable (e.g. blocked by hardware switch)
  ///
  void failed();

  ///
  /// \brief The AGV finished initiating pause/resume
  ///
  void finished();

  ///
  /// \brief Notify all active ActionHandler instances to pause (ActionHander::pause()).
  ///
  /// This can be omitted if the AGV implementation can pause actions directly
  /// (ActionHandler::paused()) has to be called anyways
  ///
  void notifyActions();

  ///
  /// \brief Notify the active NavigationHandler to pause
  ///
  /// This can be omitted if the AGV implementation can pause actions directly
  /// (NavigationHandler::pause()) has to be called anyways
  ///
  void notifyNavigation();

public:
  virtual ~PauseResumeHandler() = default;

  ///
  /// \brief The library wants the AGV to initiate a pause.
  ///
  /// The AGV has to check if it can be paused and calls (PauseResumeHandler::finished() or
  /// PauseResumeHandler::failed()) respectively.
  /// PauseResumeHandler::notifyActions() and PauseResumeHandler::notifyNavigation() can be used to
  /// notify active handlers
  ///
  virtual void doPause() = 0;

  ///
  /// \brief The library wants the AGV to resume from a pause.
  ///
  /// The AGV has to check if it can be resumed and calls (PauseResumeHandler::finished() or
  /// PauseResumeHandler::failed()) respectively.
  /// PauseResumeHandler::notifyActions() and PauseResumeHandler::notifyNavigation() can be used to
  /// notify active handlers
  ///
  virtual void doResume() = 0;
};

}  // namespace vda5050pp::interface_agv

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_PAUSE_RESUME_HANDLER */
