// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/validation/action_declared_validator.h"

#include <algorithm>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

#include "vda5050++/model/Action.h"

static const std::string k_description = "Check if the Action was declared in the AGV description";

const auto k_ser_float64_min =
    vda5050pp::interface_agv::agv_description::SerializedValue(std::numeric_limits<double>::min());

const auto k_ser_float64_max =
    vda5050pp::interface_agv::agv_description::SerializedValue(std::numeric_limits<double>::max());

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
        {"initPosition",
         {{"x", k_ser_float64_min, k_ser_float64_max, std::nullopt},
          {"y", k_ser_float64_min, k_ser_float64_max, std::nullopt},
          {"theta", k_ser_float64_min, k_ser_float64_max, std::nullopt},
          {"mapId", std::nullopt, std::nullopt, std::nullopt},
          {"lastNodeId", std::nullopt, std::nullopt, std::nullopt}},
         {},
         {vda5050pp::BlockingType::HARD},
         true,
         false,
         false},
};

vda5050pp::core::validation::ActionDeclaredValidator::ActionDeclaredValidator(
    vda5050pp::interface_agv::Handle &handle, bool ctxt_edge, bool ctxt_instant, bool ctxt_node)
    : Validator(k_description),
      handle_accessor_(handle),
      ctxt_edge_(ctxt_edge),
      ctxt_instant_(ctxt_instant),
      ctxt_node_(ctxt_node) {}

static std::list<vda5050pp::Error> paramErrors(
    const vda5050pp::Action &action,
    const vda5050pp::interface_agv::agv_description::ParameterRange &decl,
    const vda5050pp::ActionParameter &param) {
  if (decl.value_set.has_value()) {
    // Check value set constraints
    auto it = decl.value_set->find(param.value);
    if (it == decl.value_set->cend()) {
      return {{"ActionParameter Value",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
                   {"actionParameter.key", param.key},
                   {"actionParameter.value", param.value},
               }},
               {"Invalid ActionParameter Value"},
               vda5050pp::ErrorLevel::WARNING}};
    }
  } else {
    // Check ordinal constraints
    try {
      if (decl.ordinal_max.has_value()) {
        auto sparam = decl.ordinal_max.value();
        sparam.value = param.value;

        if (sparam > decl.ordinal_max.value()) {
          return {{"ActionParameterValue out of bounds",
                   {{
                       {"actionId", action.actionId},
                       {"actionType", action.actionType},
                       {"actionParameter.key", param.key},
                       {"actionParameter.value", param.value},
                   }},
                   {"Action Parameter value of of bounds"},
                   vda5050pp::ErrorLevel::WARNING}};
        }
      }
      if (decl.ordinal_min.has_value()) {
        auto sparam = decl.ordinal_min.value();
        sparam.value = param.value;

        if (sparam < decl.ordinal_min.value()) {
          return {{"ActionParameterValue out of bounds",
                   {{
                       {"actionId", action.actionId},
                       {"actionType", action.actionType},
                       {"actionParameter.key", param.key},
                       {"actionParameter.value", param.value},
                   }},
                   {"Action Parameter value of of bounds"},
                   vda5050pp::ErrorLevel::WARNING}};
        }
      }
    } catch (std::exception &e) {
      return {{"ActionParameter type",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
                   {"actionParameter.key", param.key},
                   {"actionParameter.value", param.value},
               }},
               {"Action Parameter value does not match the declared ordinal type"},
               vda5050pp::ErrorLevel::WARNING}};
    }
  }
  return {};
}

std::list<vda5050pp::Error> vda5050pp::core::validation::ActionDeclaredValidator::operator()(
    const vda5050pp::Action &action) const {
  const auto &supported_actions = this->handle_accessor_.getAGVDescription().supported_actions;

  // Only validate if supported_actions are given
  if (supported_actions.has_value()) {
    // Does the type exist?
    auto match_type = [&](auto &decl) -> bool { return decl.action_type == action.actionType; };
    auto it = std::find_if(cbegin(*supported_actions), cend(*supported_actions), match_type);

    if (it == cend(*supported_actions)) {
      it = std::find_if(cbegin(k_control_action_declarations), cend(k_control_action_declarations),
                        match_type);
      if (it == cend(k_control_action_declarations)) {
        return {{"Unknown Action",
                 {{
                     {"actionId", action.actionId},
                     {"actionType", action.actionType},
                 }},
                 {"Action Type not supported"},
                 ErrorLevel::WARNING}};
      }
    }

    // Does the context match ?
    if (this->ctxt_edge_ && !it->edge) {
      return {{"Action Context",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
               }},
               {"Action cannot be executed on Edges"},
               ErrorLevel::WARNING}};
    }
    if (this->ctxt_node_ && !it->node) {
      return {{"Action Context",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
               }},
               {"Action can!be executed on Nodes"},
               ErrorLevel::WARNING}};
    }
    if (this->ctxt_instant_ && !it->instant) {
      return {{"Action Context",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
               }},
               {"Action cannot be executed instantaneously"},
               ErrorLevel::WARNING}};
    }

    // Blocking type?
    auto match_blocking = [&](auto &b_type) { return b_type == action.blockingType; };
    if (!std::any_of(cbegin(it->blocking_types), cend(it->blocking_types), match_blocking)) {
      return {{"Action BlockingType",
               {{
                   {"actionId", action.actionId},
                   {"actionType", action.actionType},
               }},
               {"Action BlockingType not supported"},
               ErrorLevel::WARNING}};
    }

    std::list<vda5050pp::Error> errors;

    // Parameters?
    auto param_decls_left = it->parameter;
    for (const auto &param : action.actionParameters) {
      auto decl = it->parameter.find({param.key, std::nullopt, std::nullopt, std::nullopt});
      auto decl_opt =
          it->optional_parameter.find({param.key, std::nullopt, std::nullopt, std::nullopt});

      if (decl != it->parameter.cend()) {
        param_decls_left.extract(*decl);

        errors.splice(errors.begin(), paramErrors(action, *decl, param));

      } else if (decl_opt != it->optional_parameter.cend()) {
        errors.splice(errors.begin(), paramErrors(action, *decl_opt, param));
      } else {
        errors.push_back({"ActionParameter",
                          {{
                              {"actionId", action.actionId},
                              {"actionType", action.actionType},
                              {"actionParameter.key", param.key},
                          }},
                          {"Action Parameter not supported"},
                          ErrorLevel::WARNING});
      }
    }

    if (!param_decls_left.empty()) {
      std::string val;

      for (auto &param : param_decls_left) {
        val += param.key + " ";
      }

      errors.push_back({"ActionParameter missing",
                        {{
                            {"actionId", action.actionId},
                            {"actionType", action.actionType},
                            {"actionParameter.key", val},
                        }},
                        {"Required ActionParameters missing"},
                        ErrorLevel::WARNING});
    }

    return errors;
  }

  return {};
}