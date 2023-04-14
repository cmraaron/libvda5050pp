// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// TODO: Short description
//

#ifndef TEST_INCLUDE_TEST_TEST_PAUSE_RESUME_HANDLER
#define TEST_INCLUDE_TEST_TEST_PAUSE_RESUME_HANDLER

#include <optional>

#include "vda5050++/interface_agv/pause_resume_handler.h"

namespace test {

class TestPauseResumeHandler : public vda5050pp::interface_agv::PauseResumeHandler {
public:
  virtual ~TestPauseResumeHandler();
  virtual void doPause() override;
  virtual void doResume() override;
};

extern std::optional<std::reference_wrapper<TestPauseResumeHandler>> pause_resume_handler_;

}  // namespace test

#endif /* TEST_INCLUDE_TEST_TEST_PAUSE_RESUME_HANDLER */
