//  Copyright 2022 Open Logistics Foundation
//
//  Licensed under the Open Logistics License 1.0.
//  For details on the licensing terms, see the LICENSE file.

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ODOMETRY_HANDLER_H_
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ODOMETRY_HANDLER_H_

#include <chrono>
#include <memory>
#include <optional>
#include <stdexcept>
#include <thread>

#include "vda5050++/core/common/interruptable_timer.h"
#include "vda5050++/model/AGVPosition.h"
#include "vda5050++/model/Velocity.h"

namespace vda5050pp::interface_agv {
// Forward declaration
class Handle;

class OdometryHandler {
  friend class Handle;

private:
  ///
  ///\brief Pointer to the current library handle instance
  ///
  /// This cannot be a std::optional<std::reference_wrapper<Handle>> due to
  /// the lack of incomplete type support
  ///
  Handle *handle_ptr_ = nullptr;

  void setHandleRef(Handle &handle) noexcept(true);

  std::unique_ptr<std::thread> visualization_thread_;

  vda5050pp::core::common::InterruptableTimer visualization_timer_;

public:
  class InitializePositionError : public std::runtime_error {
  public:
    explicit InitializePositionError(const std::string &msg) noexcept(true);
  };

  class NotAttachedError : public std::logic_error {
  public:
    NotAttachedError() noexcept(true);
  };

  virtual ~OdometryHandler();

  ///
  ///\brief This function is called, when a initializePosition
  /// instant actions was received. When this function returns, a successful initialization
  /// is indicated. If an Exception is thrown, the exception::what member function indicates
  /// an error.
  ///
  ///\param pos the AGVPosition told by the MC
  ///\throws InitializePositionError
  ///
  virtual void initializePosition(const vda5050pp::AGVPosition &pos) noexcept(false) = 0;

  ///
  ///\brief Set the current AGVPosition (only for visualization purposes)
  ///
  /// NOTE: This does not evaluate the position. The NavigationHandler still
  /// has to process the position separately:
  ///
  ///\param pos the current AGVPosition
  ///
  void setAGVPosition(const vda5050pp::AGVPosition &pos) noexcept(false);

  ///
  ///\brief Set the current Velocity
  ///
  ///\param vel the current Velocity
  ///
  void setVelocity(const vda5050pp::Velocity &vel) noexcept(false);

  ///
  ///\brief Starts a thread, which periodically sends Visualization Messages
  ///
  ///\param period the message rate period
  ///
  void enableAutomaticVisualizationMessages(std::chrono::system_clock::duration period) noexcept(
      false);

  ///
  ///\brief If the visualization thread is running, stop it.
  ///
  void disableAutomaticVisualizationMessages() noexcept(true);

  ///
  ///\brief Check if the OdometryHandler was attached to the library Handle
  ///
  ///\return is attached?
  ///
  bool isAttached() const noexcept(true);
};

}  // namespace vda5050pp::interface_agv

#endif  // INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_ODOMETRY_HANDLER_H_
