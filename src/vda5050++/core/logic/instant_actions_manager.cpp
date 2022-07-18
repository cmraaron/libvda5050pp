// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/core/logic/instant_actions_manager.h"

#include <future>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/interface_agv/agv_description/serialized_value.h"

using namespace vda5050pp::core::logic;

InstantActionsManager::InstantActionsManager(vda5050pp::interface_agv::Handle &handle,
                                             NetManager &net_manager) noexcept(true)
    : handle_(handle), net_manager_(net_manager) {}

void InstantActionsManager::doInitPosition(const vda5050pp::Action &action) noexcept(true) {
  using SV = vda5050pp::interface_agv::agv_description::SerializedValue;

  SV x;
  x.type = vda5050pp::interface_agv::agv_description::Type::FLOAT;
  SV y;
  y.type = vda5050pp::interface_agv::agv_description::Type::FLOAT;
  SV theta;
  theta.type = vda5050pp::interface_agv::agv_description::Type::FLOAT;
  SV mapId;
  mapId.type = vda5050pp::interface_agv::agv_description::Type::STRING;
  SV lastNodeId;
  lastNodeId.type = vda5050pp::interface_agv::agv_description::Type::STRING;

  // All parameters are present, due to validation
  for (const auto &param : *action.actionParameters) {
    if (param.key == "x") {
      x.value = param.value;
    } else if (param.key == "y") {
      y.value = param.value;
    } else if (param.key == "theta") {
      theta.value = param.value;
    } else if (param.key == "mapId") {
      mapId.value = param.value;
    } else if (param.key == "lastNodeId") {
      lastNodeId.value = param.value;
    }
  }

  std::function<void()> init_task = [this, aid = action.actionId, at = action.actionType, x, y,
                                     theta, mapId, lastNodeId] {
    vda5050pp::AGVPosition pos;
    pos.x = double(x);
    pos.y = double(y);
    pos.theta = double(theta);
    pos.mapId = std::string(mapId);

    vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

    auto odom_handler = ha.getOdometryHandler();
    if (odom_handler == nullptr) {
      ha.getLogger().logWarn("No OdometryHandler set, dropping initPosition Action.");
      ha.getState().setActionStatus(aid, vda5050pp::ActionStatus::FAILED);
      ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
      return;
    }

    try {
      ha.getState().setActionStatus(aid, vda5050pp::ActionStatus::RUNNING);
      ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
      odom_handler->initializePosition(pos);
      ha.getState().setLastNode(std::string(lastNodeId));
      ha.getState().setActionStatus(aid, vda5050pp::ActionStatus::FINISHED);
    } catch (vda5050pp::interface_agv::OdometryHandler::InitializePositionError &e) {
      vda5050pp::Error err;
      err.errorType = "ActionError";
      err.errorDescription = e.what();
      err.errorReferences->push_back({"action.actionType", at});
      err.errorReferences->push_back({"action.actionId", aid});
      err.errorLevel = vda5050pp::ErrorLevel::WARNING;
      ha.getState().addError(err);
      ha.getLogger().logWarn(
          vda5050pp::core::common::format("InitializePositionError caught: {}", e.what()));
    }
    ha.getMessages().requestStateUpdate(vda5050pp::core::messages::UpdateUrgency::k_high);
  };

  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
  ha.getTaskQueue().push(std::move(init_task));
}

void InstantActionsManager::doInstantAction(const vda5050pp::Action &action) noexcept(false) {
  vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);

  if (action.actionType == "stopPause" || action.actionType == "startPause") {
    // Use PauseResumeActionManager to take care of the action
    this->pause_resume_action_manager_ =
        std::make_unique<PauseResumeActionManager>(this->handle_, action);
    this->pause_resume_action_manager_->initialize();
  } else if (action.actionType == "cancelOrder") {
    if (!this->net_manager_.isAnythingActive()) {
      // There is nothing to cancel
      ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FAILED);

      vda5050pp::Error error;
      error.errorType = "noOrderToCancel";
      error.errorDescription = "There is nothing to cancel";
      error.errorLevel = ErrorLevel::WARNING;
      error.errorReferences = {{"action.actionId", action.actionId}};
      ha.getState().addError(error);

      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_high);
      return;
    }

    // Cancel Order will skip all pending actions and cancel the running ones.
    // Finish action when the tail of the order was reached.
    this->net_manager_.onAllExited([this, action] {
      vda5050pp::core::interface_agv::HandleAccessor ha(this->handle_);
      ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FINISHED);
      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_immediate);
      ha.getState().clearOrder();
      ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_low);
      this->net_manager_.onAllExited(nullptr);
    });

    ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::RUNNING);
    ha.getMessages().requestStateUpdate(messages::UpdateUrgency::k_medium);
    this->net_manager_.cancelAll();

  } else if (action.actionType == "stateRequest") {
    // The message module will send a state update anyways, just set this action to finished
    ha.getState().setActionStatus(action.actionId, vda5050pp::ActionStatus::FINISHED);
    // TODO: maybe ask the AGV for additional info
  } else if (action.actionType == "initPosition") {
    this->doInitPosition(action);
  } else {
    // default dispatch
    this->net_manager_.interceptWithAction(action);
  }
}