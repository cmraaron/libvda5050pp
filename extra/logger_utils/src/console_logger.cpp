// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the console_logger

#include "vda5050++/extra/console_logger.h"

#include <iostream>

#include "vda5050++/extra/stream_logger.h"

using namespace vda5050pp::extra;

ConsoleLogger::ConsoleLogger(std::string_view line_prefix)
    : PrettyLogger(std::make_unique<vda5050pp::extra::StreamLogger>(
                       &std::cout, &std::cout, &std::cout, &std::cerr, &std::cerr),
                   line_prefix) {}