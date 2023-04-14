// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the TaskManager declaration
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TASK_MANAGER_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TASK_MANAGER_HPP_

#include <functional>

#include "vda5050++/core/logic/partial_net.h"
#include "vda5050++/core/logic/types.h"

/// Forward declaration of Handle, to avoid cyclic dependencies
namespace vda5050pp::interface_agv {
class Handle;
}

namespace vda5050pp::core::logic {

///
/// \brief An abstract TaskManager tied to the PTN. Manages the state of a task inside of the global
/// PTN
///
class TaskManager : public PartialNet {
protected:
  vda5050pp::interface_agv::Handle &handle_;

private:
  SeqNrT seq_;

  std::shared_ptr<Net::TransitionT> transition_initializing_fail_;
  std::shared_ptr<Net::TransitionT> transition_running_fail_;
  std::shared_ptr<Net::TransitionT> transition_paused_fail_;
  std::shared_ptr<Net::TransitionT> transition_started_;
  std::shared_ptr<Net::TransitionT> transition_pause_;
  std::shared_ptr<Net::TransitionT> transition_resume_;
  std::shared_ptr<Net::TransitionT> transition_finish_;
  std::shared_ptr<Net::TransitionT> transition_fail_;
  std::shared_ptr<Net::TransitionT> transition_disable_;
  std::shared_ptr<Net::TransitionT> transition_intercept_;
  std::shared_ptr<Net::TransitionT> transition_intercept_sequential_;
  std::shared_ptr<Net::TransitionT> transition_intercept_parallel_;
  std::shared_ptr<Net::PlaceT> place_finished_;
  std::shared_ptr<Net::PlaceT> place_failed_;
  std::shared_ptr<Net::PlaceT> place_running_;
  std::shared_ptr<Net::PlaceT> place_paused_;
  std::shared_ptr<Net::PlaceT> place_initializing_;
  std::shared_ptr<Net::PlaceT> place_entered_;
  std::shared_ptr<Net::PlaceT> place_exited_;

  ///
  ///\brief This will be called each transition, for logging purposes
  ///
  ///\param name the name of the function
  ///\param ret the successful value
  ///\return successful value
  ///
  virtual bool logTransition(const std::string &name, bool ret) noexcept(true) = 0;

  ///
  ///\brief This will be called each time a place was reached, for logging purposes
  ///
  ///\param place the place that has been reached
  ///
  virtual void logPlaceReached(const Net::PlaceT &place) noexcept(true) = 0;

  ///
  ///\brief Initialize the linked task
  ///
  virtual void taskInitialize() noexcept(true) = 0;

  ///
  ///\brief Running the linked task
  ///
  virtual void taskRunning() noexcept(true) = 0;

  ///
  ///\brief Paused the linked task
  ///
  virtual void taskPaused() noexcept(true) = 0;

  ///
  ///\brief Finished the linked task
  ///
  virtual void taskFinished() noexcept(true) = 0;

  ///
  ///\brief Failed the linked task
  ///
  virtual void taskFailed() noexcept(true) = 0;

public:
  ///
  ///\brief Create a new TaskManager which can be attached to the
  /// globale PTN
  ///
  ///\param handle the library handle
  ///\param seq the sequence id inside of the net
  ///
  TaskManager(vda5050pp::interface_agv::Handle &handle, SeqNrT seq);

  ///
  ///\brief Attach this manager to an existing net
  ///
  /// The manager will create new places and transitions and binds
  /// them to it's member functions. This should only be called once, because
  /// A manager can only be bound to one net at a time
  ///
  ///\param composed_net the net to attach to
  ///
  virtual void attachToNet(Net &composed_net) noexcept(false) override;

  ///
  ///\brief get the PTN ID for the associated readyPlace
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID readyPlace() const noexcept(true);

  ///
  ///\brief get the PTN ID for the associated donePlace
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID donePlace() const noexcept(true);

  ///
  ///\brief get the PTN ID for the associated failedPlace
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID failedPlace() const noexcept(true);

  ///
  ///\brief get the PTN ID for the associated finishedPlace
  ///
  ///\return LogicTaskNetID
  ///
  LogicTaskNetID finishedPlace() const noexcept(true);

  ///
  /// \brief get the PTN ID for the associated exitedPlace
  ///
  /// \return LogicTaskNetID
  ///
  LogicTaskNetID exitedPlace() const noexcept(true);

  ///
  /// \brief get the PTN ID for the associated intercepting output
  /// which yields the flow to the intercepting task
  ///
  /// \return LogicTaskNetID
  ///
  LogicTaskNetID interceptingBeginPlace() const noexcept(true);

  ///
  /// \brief get the PTN ID for the associated intercepting input
  /// which yields the flow back to this task
  ///
  /// \return LogicTaskNetID
  ///
  LogicTaskNetID interceptingEndPlace() const noexcept(true);

  ///
  /// \brief Notify when the task was exited
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  /// \param fn the function to notify
  ///
  void onExited(std::function<void(LogicTaskNetID exit_id)> fn) noexcept(true);

  ///
  ///\brief Try to reach the paused state of the net
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successful
  ///
  bool paused() noexcept(true);

  ///
  ///\brief Try to reach the resumed state of the net
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successful
  ///
  bool resumed() noexcept(true);

  ///
  ///\brief Try to reach the started state of the net
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successful
  ///
  bool started() noexcept(true);

  ///
  ///\brief Try to reach the finished state of the net
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successful
  ///
  bool finished() noexcept(true);

  ///
  ///\brief Try to reach the failed state of the net
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successful
  ///
  bool failed() noexcept(true);

  ///
  ///\brief Check if the Task is running
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isRunning() noexcept(true);

  ///
  ///\brief Check if the Task is paused
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isPaused() noexcept(true);

  ///
  ///\brief Check if the Task is failed
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isFailed() noexcept(true);

  ///
  ///\brief Check if the Task is finished
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isFinished() noexcept(true);

  ///
  ///\brief Check if the Task is done
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isDone() noexcept(true);

  ///
  ///\brief Check if the Task is active
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isActive() noexcept(true);

  ///
  ///\brief Check if the Task is initializing
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return is it?
  ///
  bool isInitializing() noexcept(true);

  ///
  ///\brief Cancel the current Task.
  /// Once it is reached, it will immediately fail.
  /// This is used to cancel a complete order
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return could the action be canceled? false, if it already ran
  ///
  bool cancel() noexcept(true);

  ///
  /// \brief Intercept the current Task.
  /// The task will not be able to reach the done place on it's own.
  /// interceptParallel() or interceptSequential() must be called to
  /// enable intercepting.
  /// This should be used to lock the task for interception, to avoid it beeing exited.
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  /// \return could the task be intercepted?
  ///
  bool intercept() noexcept(true);

  ///
  /// \brief Intercept the current Task.
  ///
  /// The Task will not be able to reach the done place on it's own (exit can be reached).
  /// A new subnet can be attached to the interceptingBegin place. On success
  /// the interceptingBegin place has a token. The original done place of this task
  /// is reached, once this and the intercepting are finished or failed.
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  /// \return could the action be intercepted?
  ///
  bool interceptParallel() noexcept(true);

  ///
  /// \brief Intercept the current Task.
  ///
  /// The Task will not be able to reach the done place on it's own (exit can be reached).
  /// A new subnet can be attached to the interceptingBegin place. The
  /// interceptingBegin place is reached once this task is finished of failed. The original done
  /// place of this task is reached, once the intercepting is finished or failed.
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  /// \return could the action be intercepted?
  ///
  bool interceptSequential() noexcept(true);

  ///
  ///\brief Try to pause the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  virtual void pause() noexcept(true) = 0;

  ///
  ///\brief Try to resume the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  virtual void resume() noexcept(true) = 0;

  ///
  ///\brief Stop the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successfull
  ///
  virtual void stop() noexcept(true) = 0;

  ///
  ///\brief Get the sequence number of this task
  ///
  ///\return SeqNrT
  ///
  SeqNrT getSeq() const noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TASK_MANAGER_HPP_
