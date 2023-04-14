// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
//

#ifndef EXTRA_JSON_MODEL_INCLUDE_VDA5050_2B_2B_EXTRA_JSON_MODEL
#define EXTRA_JSON_MODEL_INCLUDE_VDA5050_2B_2B_EXTRA_JSON_MODEL

#include <vda5050++/model/Connection.h>
#include <vda5050++/model/InstantActions.h>
#include <vda5050++/model/Order.h>
#include <vda5050++/model/State.h>
#include <vda5050++/model/Visualization.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace vda5050pp {

void to_json(json &j, const Header &d);

void from_json(const json &j, Header &d);

void to_json(json &j, const ConnectionState &d);
void from_json(const json &j, ConnectionState &d);

void to_json(json &j, const Connection &d);
void from_json(const json &j, Connection &d);

void to_json(json &j, const AGVPosition &d);
void from_json(const json &j, AGVPosition &d);

void to_json(json &j, const Velocity &d);
void from_json(const json &j, Velocity &d);

void to_json(json &j, const Visualization &d);
void from_json(const json &j, Visualization &d);

void to_json(json &j, const ActionParameter &d);
void from_json(const json &j, ActionParameter &d);

void to_json(json &j, const BlockingType &d);
void from_json(const json &j, BlockingType &d);

void to_json(json &j, const Action &d);
void from_json(const json &j, Action &d);

void to_json(json &j, const NodePosition &d);
void from_json(const json &j, NodePosition &d);

void to_json(json &j, const Node &d);
void from_json(const json &j, Node &d);

void to_json(json &j, const ControlPoint &d);
void from_json(const json &j, ControlPoint &d);

void to_json(json &j, const Trajectory &d);
void from_json(const json &j, Trajectory &d);

void to_json(json &j, const Edge &d);
void from_json(const json &j, Edge &d);

void to_json(json &j, const Order &d);
void from_json(const json &j, Order &d);

void to_json(json &j, const InstantActions &d);
void from_json(const json &j, InstantActions &d);

void to_json(json &j, const ActionStatus &d);
void from_json(const json &j, ActionStatus &d);

void to_json(json &j, const ActionState &d);
void from_json(const json &j, ActionState &d);

void to_json(json &j, const BatteryState &d);
void from_json(const json &j, BatteryState &d);

void to_json(json &j, const EdgeState &d);
void from_json(const json &j, EdgeState &d);

void to_json(json &j, const NodeState &d);
void from_json(const json &j, NodeState &d);

void to_json(json &j, const ErrorReference &d);
void from_json(const json &j, ErrorReference &d);

void to_json(json &j, const ErrorLevel &d);
void from_json(const json &j, ErrorLevel &d);

void to_json(json &j, const Error &d);
void from_json(const json &j, Error &d);

void to_json(json &j, const InfoReference &d);
void from_json(const json &j, InfoReference &d);

void to_json(json &j, const InfoLevel &d);
void from_json(const json &j, InfoLevel &d);

void to_json(json &j, const Info &d);
void from_json(const json &j, Info &d);

void to_json(json &j, const BoundingBoxReference &d);
void from_json(const json &j, BoundingBoxReference &d);

void to_json(json &j, const LoadDimensions &d);
void from_json(const json &j, LoadDimensions &d);

void to_json(json &j, const Load &d);
void from_json(const json &j, Load &d);

void to_json(json &j, const SafetyState &d);
void from_json(const json &j, SafetyState &d);

void to_json(json &j, const State &d);
void from_json(const json &j, State &d);

void to_json(json &j, const EStop &d);
void from_json(const json &j, EStop &d);

void to_json(json &j, const OperatingMode &d);
void from_json(const json &j, OperatingMode &d);

}  // namespace vda5050pp

#endif /* EXTRA_JSON_MODEL_INCLUDE_VDA5050_2B_2B_EXTRA_JSON_MODEL */
