// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// TODO: Short description
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_SINGLE_STEP_TASK
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_SINGLE_STEP_TASK

#include "vda5050++/core/logic/task_manager.h"

namespace vda5050pp::core::logic {

class ContinuousNavigationManager;

class ContinuousNavigationSingleStepTask : public TaskManager {
private:
  ContinuousNavigationManager &manager_;
  uint32_t node_seq_;

  ///
  ///\brief This will be called each transition, for logging purposes
  ///
  ///\param name the name of the function
  ///\param ret the successful value
  ///\return successful value
  ///
  virtual bool logTransition(const std::string &name, bool ret) noexcept(true) override;

  ///
  ///\brief This will be called each time a place was reached, for logging purposes
  ///
  ///\param place the place that has been reached
  ///
  virtual void logPlaceReached(const Net::PlaceT &place) noexcept(true) override;

  ///
  ///\brief Initialize the linked task
  ///
  virtual void taskInitialize() noexcept(true) override;

  ///
  ///\brief Running the linked task
  ///
  virtual void taskRunning() noexcept(true) override;

  ///
  ///\brief Paused the linked task
  ///
  virtual void taskPaused() noexcept(true) override;

  ///
  ///\brief Finished the linked task
  ///
  virtual void taskFinished() noexcept(true) override;

  ///
  ///\brief Failed the linked task
  ///
  virtual void taskFailed() noexcept(true) override;

public:
  ContinuousNavigationSingleStepTask(vda5050pp::interface_agv::Handle &handle,
                                     ContinuousNavigationManager &manager, uint32_t node_seq_id,
                                     SeqNrT seq);
  ///
  ///\brief Try to pause the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  virtual void pause() noexcept(true) override;

  ///
  ///\brief Try to resume the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  virtual void resume() noexcept(true) override;

  ///
  ///\brief Stop the associated task. (should be called from the controller layer)
  ///
  /// NOTE: the manager hat to be attached to a net before (mgr.attachToNet())
  ///
  ///\return successfull
  ///
  virtual void stop() noexcept(true) override;

  uint32_t getNodeSequence() const noexcept(true);
};

}  // namespace vda5050pp::core::logic

#endif /* INCLUDE_VDA5050_2B_2B_CORE_LOGIC_CONTINUOUS_NAVIGATION_SINGLE_STEP_TASK */
