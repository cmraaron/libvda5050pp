// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "test/test_pause_resume_handler.h"

#include <functional>

test::TestPauseResumeHandler::~TestPauseResumeHandler() {
  test::pause_resume_handler_ = std::nullopt;
}

void test::TestPauseResumeHandler::doPause() {
  test::pause_resume_handler_ = std::ref(*this);
  this->notifyActions();
  this->notifyNavigation();
}

void test::TestPauseResumeHandler::doResume() {
  test::pause_resume_handler_ = std::ref(*this);
  this->notifyActions();
  this->notifyNavigation();
}

std::optional<std::reference_wrapper<test::TestPauseResumeHandler>> test::pause_resume_handler_;