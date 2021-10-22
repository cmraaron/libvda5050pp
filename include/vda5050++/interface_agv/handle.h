// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
//  This file contatins the state of the libvda5050pp
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_HANDLE
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_HANDLE

#include <condition_variable>
#include <list>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "vda5050++/core/common/blocking_queue.h"
#include "vda5050++/core/logic/logic.h"
#include "vda5050++/core/messages/messages.h"
#include "vda5050++/core/state/state_manager.h"
#include "vda5050++/core/validation/validation_provider.h"
#include "vda5050++/interface_agv/action_handler.h"
#include "vda5050++/interface_agv/agv_description/agv_description.h"
#include "vda5050++/interface_agv/continuous_navigation_handler.h"
#include "vda5050++/interface_agv/logger.h"
#include "vda5050++/interface_agv/pause_resume_handler.h"
#include "vda5050++/interface_agv/step_based_navigation_handler.h"
#include "vda5050++/interface_mc/connector.h"
#include "vda5050++/interface_mc/connector_passive.h"

/// Forward declaration of Handle accessor
namespace vda5050pp::core::interface_agv {
class HandleAccessor;
class ConstHandleAccessor;
}  // namespace vda5050pp::core::interface_agv

namespace vda5050pp::interface_agv {

///
/// \brief Library Handler configuration structure
///
/// \tparam NavigationHandler the class type for the navigation handle (deriving
/// vda5050pp::interface_agv::StepBasedNavigationHandler or
/// vda5050pp::interface_agv::ContinuousNavigationHandler)
/// \tparam ActionHandler the class type for
/// the action handler (deriving vda5050pp::interface_agv::ActionHandler)
/// \tparam PauseResumeHandler the class type for the pause resume handler
/// the action handler (deriving vda5050pp::interface_agv::PauseResumeHandler)
///
template <typename NavigationHandler, typename ActionHandler, typename PauseResumeHandler>
struct Handlers {
  using NavigationHandler_ = NavigationHandler;
  using ActionHandler_ = ActionHandler;
  using PauseResumeHandler_ = PauseResumeHandler;
};

///
///\brief This represents the whole state of the library
///
/// all members are private and should only be accessed by the library (via friends).
/// In certain edge cases (when some functionality is not provided by the library)
/// the state may be accessed from inside of the user-code (again via friends)
/// @see vda5050pp::core::interface_agv::HandleAccessor
///
class Handle {
public:
  friend class vda5050pp::core::interface_agv::HandleAccessor;
  friend class vda5050pp::core::interface_agv::ConstHandleAccessor;

  enum class SpinStatus {
    k_ok,
    k_shutdown,
  };

  ///
  ///\brief Create new Interface-Handle
  ///
  /// This is the only object which has to be instanciated by the user of
  /// this library. All configuration can be done via member functions
  ///
  ///\param agv_description the vehicle description to refer to
  ///
  template <typename Handlers, typename Connector>
  explicit Handle(const agv_description::AGVDescription &agv_description,
                  std::shared_ptr<Connector> connector, Handlers,
                  std::shared_ptr<vda5050pp::interface_agv::Logger> logger = nullptr)
      : shutdown_((Logger::setCurrentLogger(logger),
                   false)),  // Workaround to initialize the logger before any component
        create_action_handler_(std::make_shared<typename Handlers::ActionHandler_>),
        create_pause_resume_handler_(std::make_shared<typename Handlers::PauseResumeHandler_>),
        agv_description_(agv_description),
        logic_(*this),
        validation_provider_(*this),
        messages_(*this) {
    using namespace std::chrono_literals;
    this->state_update_period_ = 30s;

    // Check Action and PauseResume Handler ////////////////////////////////////
    static_assert(
        std::is_base_of_v<ActionHandler, typename Handlers::ActionHandler_>,
        "Handler::ActionHandler must inherit from vda5050pp::interface_agv::ActionHandler");
    static_assert(std::is_default_constructible_v<typename Handlers::ActionHandler_>,
                  "Handler::ActionHandler must be default constructible");
    static_assert(std::is_base_of_v<PauseResumeHandler, typename Handlers::PauseResumeHandler_>,
                  "Handler::PauseResumeHandler must inherit from "
                  "vda5050pp::interface_agv::PauseResumeHandler");
    static_assert(std::is_default_constructible_v<typename Handlers::PauseResumeHandler_>,
                  "Handler::PauseResumeHandler must be default constructible");
    this->create_action_handler_ = std::make_shared<typename Handlers::ActionHandler_>;
    this->create_pause_resume_handler_ = std::make_shared<typename Handlers::PauseResumeHandler_>;

    // Check NavigationHandler /////////////////////////////////////////////////
    constexpr auto nav_dc = std::is_default_constructible_v<typename Handlers::NavigationHandler_>;
    constexpr auto nav_cn = std::is_base_of_v<vda5050pp::interface_agv::ContinuousNavigationHandler,
                                              typename Handlers::NavigationHandler_>;
    constexpr auto nav_tn = std::is_base_of_v<vda5050pp::interface_agv::StepBasedNavigationHandler,
                                              typename Handlers::NavigationHandler_>;
    static_assert(nav_cn || nav_tn, "Handlers::NavigationHandler neither inherits from "
                                    "vda5050pp::interface_agv::ContinuousNavigationHandler nor "
                                    "from vda5050pp::interface_agv::StepBasedNavigationHandler");
    static_assert(nav_dc, "Handlers::NavigationHandler must be default constructible");
    if constexpr (nav_cn) {
      this->create_continuous_navigation_handler_ =
          std::make_shared<typename Handlers::NavigationHandler_>;
    } else if constexpr (nav_tn) {
      this->create_navigate_to_node_handler_ =
          std::make_shared<typename Handlers::NavigationHandler_>;
    }

    // Check Connector /////////////////////////////////////////////////////////
    static_assert(std::is_base_of_v<vda5050pp::interface_mc::Connector, Connector>,
                  "Connector must inherit from "
                  "vda5050pp::interface_mc::Connector (or "
                  "vda5050pp::interface_agv::ConnectorPassive");
    if constexpr (std::is_base_of_v<vda5050pp::interface_mc::ConnectorPassive, Connector>) {
      this->connector_passive_ = connector;
      this->connector_ = this->connector_passive_;
    } else {
      this->connector_ = connector;
    }

    this->messages_.connect();
  }

  ///\brief disallow copy
  Handle(const Handle &handle) = delete;
  ///\brief disallow move
  Handle(Handle &&handle) = delete;
  ///\brief disallow copy
  void operator=(const Handle &handle) = delete;
  ///\brief disallow move
  void operator=(Handle &&handle) = delete;

  ///
  ///\brief shutdown the library @see shutdown().
  /// NOTE: This blocks until the library was shut down.
  ///
  ~Handle();

  ///
  ///\brief Set the maximum delay between state updates (default: 30s)
  ///
  ///\param period the period length
  ///
  void setStateUpdatePeriod(const std::chrono::system_clock::duration &period);

  ///
  ///\brief Set the current logger. This sets the static Logger::current_logger_ instance.
  ///
  ///\param logger
  ///
  void setLogger(std::shared_ptr<vda5050pp::interface_agv::Logger> logger) const;

  ///
  /// \brief Evaluate library calls and poll messages (if passive), until the library is shut down.
  /// NOTE: can be called by multiple threads for parallel handling (given
  /// ConnectorPassive::spinOnce may run in parallel). Returns when the library is shut down
  ///
  void spin() noexcept(true);

  ///
  /// \brief set the number of threads, which spin the library in parallel. Deletes or creates
  /// threads to reach num_threads. (non-blocking)
  ///
  /// \param num_threads target number of threads
  ///
  void spinParallel(uint16_t num_threads) noexcept(true);

  ///
  /// \brief Spin once (poll message if passive and run one handle callback) if available
  ///
  /// \return SpinStatus tells if the library could spin
  ///
  SpinStatus spinOnce() noexcept(true);

  ///
  /// \brief Spin until all library calls are evaluated (polling messages in between, if passive
  /// connector is set)
  ///
  /// \return SpinStatus tells if the library could spin
  ///
  SpinStatus spinAll() noexcept(true);

  ///
  /// \brief Shutdown the library, cancelling all running tasks and disconnect from MC
  ///
  void shutdown() noexcept(true);

  ///
  ///\brief get a constref to the current AGV description
  ///
  ///\return const vda5050pp::interface_agv::agv_description::AGVDescription&
  ///
  const vda5050pp::interface_agv::agv_description::AGVDescription &getAGVDescription() const {
    return this->agv_description_;
  }

private:
  class Spinner final {
  private:
    bool stop_ = false;
    Handle &handle_;
    std::thread thread_;
    void spin();

  public:
    explicit Spinner(Handle &handle);
    ~Spinner();
  };

  ///\brief indicates shutdown of the library
  bool shutdown_;

  std::vector<std::unique_ptr<Spinner>> spinners_;

  ///\brief the current library connector
  std::shared_ptr<vda5050pp::interface_mc::Connector> connector_;

  ///\brief if the library connector is passive, this will point to connector_ too
  std::shared_ptr<vda5050pp::interface_mc::ConnectorPassive> connector_passive_;

  ///\brief period of automatic state updates
  std::chrono::system_clock::duration state_update_period_;

  ///\brief functor for creating user handles
  std::function<std::shared_ptr<StepBasedNavigationHandler>()> create_navigate_to_node_handler_;

  ///\brief functor for creating user handles
  std::function<std::shared_ptr<ContinuousNavigationHandler>()>
      create_continuous_navigation_handler_;

  ///\brief functor for creating user handles
  std::function<std::shared_ptr<ActionHandler>()> create_action_handler_;

  ///\brief functor for creating user handles
  std::function<std::shared_ptr<PauseResumeHandler>()> create_pause_resume_handler_;

  ///
  /// \brief The CallbackQueue contains all calls to Handlers, the are run by calling the spin
  /// functions
  ///
  vda5050pp::core::common::BlockingQueue<std::function<void()>> task_queue_;

  ///
  ///\brief The description of the AGV using this library
  ///
  agv_description::AGVDescription agv_description_;

  ///\brief state module
  vda5050pp::core::state::StateManager state_manager_;

  ///\brief logic module
  vda5050pp::core::logic::Logic logic_;

  ///\brief validation module
  vda5050pp::core::validation::ValidationProvider validation_provider_;

  ///\brief message module
  vda5050pp::core::messages::Messages messages_;
};

}  // namespace vda5050pp::interface_agv

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_HANDLE */
