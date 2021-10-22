// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the ControlActionDeclarations
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_CONTROL_ACTION_DECLARATIONS_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_CONTROL_ACTION_DECLARATIONS_HPP_

#include <set>

#include "vda5050++/interface_agv/agv_description/action_declaration.h"

namespace vda5050pp::core::validation {

///
/// \brief The set of all control actions
///
const std::set<vda5050pp::interface_agv::agv_description::ActionDeclaration>
    k_control_action_declarations = {
        {"startPause", {}, {}, {vda5050pp::BlockingType::HARD}, true, false, false},
        {"stopPause", {}, {}, {vda5050pp::BlockingType::HARD}, true, false, false},
        {"stateRequest", {}, {}, {vda5050pp::BlockingType::NONE}, true, false, false},
        {"logReport",
         {{"reason", std::nullopt, std::nullopt, std::nullopt}},
         {},
         {vda5050pp::BlockingType::NONE},
         true,
         false,
         false},
        {"cancelOrder", {}, {}, {vda5050pp::BlockingType::HARD}, true, false, false},
};

}  // namespace vda5050pp::core::validation

#endif  // INCLUDE_VDA5050_2B_2B_CORE_VALIDATION_CONTROL_ACTION_DECLARATIONS_HPP_
