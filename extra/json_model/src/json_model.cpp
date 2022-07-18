// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
//

#include "vda5050++/extra/json_model.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace vda5050pp {

void to_json(json &j, const Header &d) {
  j["headerId"] = d.headerId;

  // Write ISO8601 UTC timestamp
  auto tt = std::chrono::system_clock::to_time_t(d.timestamp);
  std::stringstream ss;
  std::tm tm;
  ss << std::put_time(gmtime_r(&tt, &tm), "%FT%TZ");
  j["timestamp"] = ss.str();

  j["version"] = d.version;
  j["manufacturer"] = d.manufacturer;
  j["serialNumber"] = d.serialNumber;
}

void from_json(const json &j, Header &d) {
  d.headerId = j.at("headerId");

  // Parse ISO8601 UTC timestamp
  using timestampT = decltype(vda5050pp::Header::timestamp);
  std::string timestamp_str = j.at("timestamp");
  std::stringstream ss(timestamp_str);
  std::tm tm;
  ss >> std::get_time(&tm, "%FT%TZ");
  d.timestamp = timestampT::clock::from_time_t(std::mktime(&tm));

  d.version = j.at("version");
  d.manufacturer = j.at("manufacturer");
  d.serialNumber = j.at("serialNumber");
}

void to_json(json &j, const ConnectionState &d) {
  switch (d) {
    case ConnectionState::ONLINE:
      j = "ONLINE";
      break;
    case ConnectionState::OFFLINE:
      j = "OFFLINE";
      break;
    case ConnectionState::CONNECTIONBROKEN:
      j = "CONNECTIONBROKEN";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, ConnectionState &d) {
  auto str = j.get<std::string>();
  if (str == "ONLINE") {
    d = ConnectionState::ONLINE;
  } else if (str == "OFFLINE") {
    d = ConnectionState::OFFLINE;
  } else if (str == "CONNECTIONBROKEN") {
    d = ConnectionState::CONNECTIONBROKEN;
  }
}

void to_json(json &j, const Connection &d) {
  to_json(j, d.header);
  j["connectionState"] = d.connectionState;
}
void from_json(const json &j, Connection &d) {
  from_json(j, d.header);
  j.at("connectionState").get_to(d.connectionState);
}

void to_json(json &j, const AGVPosition &d) {
  j["positionInitialized"] = d.positionInitialized;
  j["x"] = d.x;
  j["y"] = d.y;
  j["theta"] = d.theta;
  if (d.deviationRange.has_value()) {
    j["deviationRange"] = *d.deviationRange;
  }
  if (d.localizationScore.has_value()) {
    j["localizationScore"] = *d.localizationScore;
  }
  j["mapId"] = d.mapId;
  if (d.mapDescription.has_value()) {
    j["mapDescription"] = *d.mapDescription;
  }
}
void from_json(const json &j, AGVPosition &d) {
  j.at("positionInitialized").get_to(d.positionInitialized);
  j.at("x").get_to(d.x);
  j.at("y").get_to(d.y);
  j.at("theta").get_to(d.theta);
  if (j.contains("deviationRange")) {
    d.deviationRange = j.at("deviationRange");
  }
  if (j.contains("localizationScore")) {
    d.localizationScore = j.at("localizationScore");
  }
  j.at("mapId").get_to(d.mapId);
  if (j.contains("mapDescription")) {
    d.mapDescription = j.at("mapDescription");
  }
}

void to_json(json &j, const Velocity &d) {
  if (d.vx.has_value()) {
    j["vx"] = *d.vx;
  }
  if (d.vy.has_value()) {
    j["vy"] = *d.vy;
  }
  if (d.omega.has_value()) {
    j["omega"] = *d.omega;
  }
}
void from_json(const json &j, Velocity &d) {
  if (j.contains("omega")) {
    d.omega = j.at("omega");
  }
  if (j.contains("vx")) {
    d.vx = j.at("vx");
  }
  if (j.contains("vy")) {
    d.vy = j.at("vy");
  }
}

void to_json(json &j, const Visualization &d) {
  to_json(j, d.header);
  j["agvPosition"] = d.agvPosition;
  j["velocity"] = d.velocity;
}
void from_json(const json &j, Visualization &d) {
  from_json(j, d.header);
  j.at("agvPosition").get_to(d.agvPosition);
  j.at("velocity").get_to(d.velocity);
}

void to_json(json &j, const ActionParameter &d) {
  j["key"] = d.key;
  j["value"] = d.value;
}
void from_json(const json &j, ActionParameter &d) {
  d.key = j.at("key");
  d.value = j.at("value");
}

void to_json(json &j, const BlockingType &d) {
  switch (d) {
    case BlockingType::SOFT:
      j = "SOFT";
      break;
    case BlockingType::HARD:
      j = "HARD";
      break;
    case BlockingType::NONE:
      j = "NONE";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, BlockingType &d) {
  auto str = j.get<std::string>();
  if (str == "SOFT") {
    d = BlockingType::SOFT;
  } else if (str == "HARD") {
    d = BlockingType::HARD;
  } else if (str == "NONE") {
    d = BlockingType::NONE;
  }
}

void to_json(json &j, const Action &d) {
  if (d.actionDescription.has_value()) {
    j["actionDescription"] = *d.actionDescription;
  }
  j["actionId"] = d.actionId;
  j["actionParameters"] = d.actionParameters;
  j["actionType"] = d.actionType;
  j["blockingType"] = d.blockingType;
}
void from_json(const json &j, Action &d) {
  if (j.contains("actionDescription")) {
    d.actionDescription = j.at("actionDescription");
  }
  d.actionId = j.at("actionId");
  d.actionParameters = j.at("actionParameters").get<std::vector<ActionParameter>>();
  d.actionType = j.at("actionType");
  d.blockingType = j.at("blockingType");
}

void to_json(json &j, const NodePosition &d) {
  if (d.allowedDeviationTheta.has_value()) {
    j["allowedDeviationTheta"] = *d.allowedDeviationTheta;
  }
  if (d.allowedDeviationXY.has_value()) {
    j["allowedDeviationXY"] = *d.allowedDeviationXY;
  }
  if (d.mapDescription.has_value()) {
    j["mapDescription"] = *d.mapDescription;
  }
  j["mapId"] = d.mapId;
  if (d.theta.has_value()) {
    j["theta"] = *d.theta;
  }
  j["x"] = d.x;
  j["y"] = d.y;
}
void from_json(const json &j, NodePosition &d) {
  if (j.contains("allowedDeviationTheta")) {
    d.allowedDeviationTheta = j.at("allowedDeviationTheta");
  }
  if (j.contains("allowedDeviationXY")) {
    d.allowedDeviationXY = j.at("allowedDeviationXY");
  }
  if (j.contains("mapDescription")) {
    d.mapDescription = j.at("mapDescription");
  }
  d.mapId = j.at("mapId");
  if (j.contains("theta")) {
    d.theta = j.at("theta");
  }
  d.x = j.at("x");
  d.y = j.at("y");
}

void to_json(json &j, const Node &d) {
  j["actions"] = d.actions;
  if (d.nodeDescription.has_value()) {
    j["nodeDescription"] = *d.nodeDescription;
  }
  j["nodeId"] = d.nodeId;
  if (d.nodePosition.has_value()) {
    j["nodePosition"] = *d.nodePosition;
  }
  j["released"] = d.released;
  j["sequenceId"] = d.sequenceId;
}
void from_json(const json &j, Node &d) {
  d.actions = j.at("actions").get<std::vector<Action>>();
  if (j.contains("nodeDescription")) {
    d.nodeDescription = j.at("nodeDescription");
  }
  d.nodeId = j.at("nodeId");
  if (j.contains("nodePosition")) {
    d.nodePosition = j.at("nodePosition");
  }
  d.released = j.at("released");
  d.sequenceId = j.at("sequenceId");
}

void to_json(json &j, const ControlPoint &d) {
  if (d.orientation.has_value()) {
    j["orientation"] = *d.orientation;
  }
  j["weight"] = d.weight;
  j["x"] = d.x;
  j["y"] = d.y;
}
void from_json(const json &j, ControlPoint &d) {
  if (j.contains("orientation")) {
    d.orientation = j.at("orientation");
  }
  d.weight = j.at("weight");
  d.x = j.at("x");
  d.y = j.at("y");
}

void to_json(json &j, const Trajectory &d) {
  j["controlPoints"] = d.controlPoints;
  j["degree"] = d.degree;
  j["knotVector"] = d.knotVector;
}
void from_json(const json &j, Trajectory &d) {
  d.controlPoints = j.at("controlPoints").get<std::vector<ControlPoint>>();
  d.degree = j.at("degree");
  d.knotVector = j.at("knotVector").get<std::vector<double>>();
}

void to_json(json &j, const Edge &d) {
  j["actions"] = d.actions;
  if (d.direction.has_value()) {
    j["direction"] = *d.direction;
  }
  if (d.edgeDescription.has_value()) {
    j["edgeDescription"] = *d.edgeDescription;
  }
  j["edgeId"] = d.edgeId;
  j["endNodeId"] = d.endNodeId;
  if (d.length.has_value()) {
    j["length"] = *d.length;
  }
  if (d.maxHeight.has_value()) {
    j["maxHeight"] = *d.maxHeight;
  }
  if (d.maxRotationSpeed.has_value()) {
    j["maxRotationSpeed"] = *d.maxRotationSpeed;
  }
  if (d.maxSpeed.has_value()) {
    j["maxSpeed"] = *d.maxSpeed;
  }
  if (d.minHeight.has_value()) {
    j["minHeight"] = *d.minHeight;
  }
  if (d.orientation.has_value()) {
    j["orientation"] = *d.orientation;
  }
  j["released"] = d.released;
  if (d.rotationAllowed.has_value()) {
    j["rotationAllowed"] = *d.rotationAllowed;
  }
  j["sequenceId"] = d.sequenceId;
  j["startNodeId"] = d.startNodeId;
  if (d.trajectory.has_value()) {
    j["trajectory"] = *d.trajectory;
  }
}
void from_json(const json &j, Edge &d) {
  d.actions = j.at("actions").get<std::vector<Action>>();
  if (j.contains("direction")) {
    d.direction = j.at("direction");
  }
  if (j.contains("edgeDescription")) {
    d.edgeDescription = j.at("edgeDescription");
  }
  d.edgeId = j.at("edgeId");
  d.endNodeId = j.at("endNodeId");
  if (j.contains("length")) {
    d.length = j.at("length");
  }
  if (j.contains("maxHeight")) {
    d.maxHeight = j.at("maxHeight");
  }
  if (j.contains("maxRotationSpeed")) {
    d.maxRotationSpeed = j.at("maxRotationSpeed");
  }
  if (j.contains("maxSpeed")) {
    d.maxSpeed = j.at("maxSpeed");
  }
  if (j.contains("minHeight")) {
    d.minHeight = j.at("minHeight");
  }
  if (j.contains("orientation")) {
    d.orientation = j.at("orientation");
  }
  d.released = j.at("released");
  if (j.contains("rotationAllowed")) {
    d.rotationAllowed = j.at("rotationAllowed");
  }
  d.sequenceId = j.at("sequenceId");
  d.startNodeId = j.at("startNodeId");
  if (j.contains("trajectory")) {
    d.trajectory = j.at("trajectory");
  }
}

void to_json(json &j, const Order &d) {
  to_json(j, d.header);
  j["edges"] = d.edges;
  j["nodes"] = d.nodes;
  j["orderId"] = d.orderId;
  j["orderUpdateId"] = d.orderUpdateId;
  if (d.zoneSetId.has_value()) {
    j["zoneSetId"] = *d.zoneSetId;
  }
}
void from_json(const json &j, Order &d) {
  from_json(j, d.header);
  d.edges = j.at("edges").get<std::vector<Edge>>();
  d.nodes = j.at("nodes").get<std::vector<Node>>();
  d.orderId = j.at("orderId");
  d.orderUpdateId = j.at("orderUpdateId");
  if (j.contains("zoneSetId")) {
    d.zoneSetId = j.at("zoneSetId");
  }
}

void to_json(json &j, const InstantActions &d) {
  to_json(j, d.header);
  j["instantActions"] = d.instantActions;
}
void from_json(const json &j, InstantActions &d) {
  from_json(j, d.header);
  d.instantActions = j.at("instantActions").get<std::vector<Action>>();
}

void to_json(json &j, const ActionStatus &d) {
  switch (d) {
    case ActionStatus::FINISHED:
      j = "FINISHED";
      break;
    case ActionStatus::INITIALIZING:
      j = "INITIALIZING";
      break;
    case ActionStatus::PAUSED:
      j = "PAUSED";
      break;
    case ActionStatus::RUNNING:
      j = "RUNNING";
      break;
    case ActionStatus::WAITING:
      j = "WAITING";
      break;
    case ActionStatus::FAILED:
      j = "FAILED";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, ActionStatus &d) {
  auto str = j.get<std::string>();
  if (str == "FAILED") {
    d = ActionStatus::FAILED;
  } else if (str == "FINISHED") {
    d = ActionStatus::FINISHED;
  } else if (str == "INITIALZING") {
    d = ActionStatus::INITIALIZING;
  } else if (str == "PAUSED") {
    d = ActionStatus::PAUSED;
  } else if (str == "RUNNING") {
    d = ActionStatus::RUNNING;
  } else if (str == "WAITING") {
    d = ActionStatus::WAITING;
  }
}

void to_json(json &j, const ActionState &d) {
  if (d.actionDescription.has_value()) {
    j["actionDescription"] = *d.actionDescription;
  }
  j["actionId"] = d.actionId;
  j["actionStatus"] = d.actionStatus;
  if (d.actionType.has_value()) {
    j["actionType"] = *d.actionType;
  }
  if (d.resultDescription.has_value()) {
    j["resultDescription"] = *d.resultDescription;
  }
}
void from_json(const json &j, ActionState &d) {
  if (j.contains("actionDescription")) {
    d.actionDescription = j.at("actionDescription");
  }
  d.actionId = j.at("actionId");
  d.actionStatus = j.at("actionStatus");
  if (j.contains("actionType")) {
    d.actionType = j.at("actionType");
  }
  if (j.contains("resultDescription")) {
    d.resultDescription = j.at("resultDescription");
  }
}

void to_json(json &j, const BatteryState &d) {
  j["batteryCharge"] = d.batteryCharge;
  if (d.batteryHealth.has_value()) {
    j["batteryHealth"] = *d.batteryHealth;
  }
  if (d.batteryVoltage.has_value()) {
    j["batteryVoltage"] = *d.batteryVoltage;
  }
  j["charging"] = d.charging;
  if (d.reach.has_value()) {
    j["reach"] = *d.reach;
  }
}
void from_json(const json &j, BatteryState &d) {
  d.batteryCharge = j.at("batteryCharge");
  if (j.contains("batteryHealth")) {
    d.batteryHealth = j.at("batteryHealth");
  }
  if (j.contains("batteryVoltage")) {
    d.batteryVoltage = j.at("batteryVoltage");
  }
  d.charging = j.at("charging");
  if (j.contains("reach")) {
    d.reach = j.at("reach");
  }
}

void to_json(json &j, const EdgeState &d) {
  if (d.edgeDescription.has_value()) {
    j["edgeDescription"] = *d.edgeDescription;
  }
  j["edgeId"] = d.edgeId;
  j["released"] = d.released;
  j["sequenceId"] = d.sequenceId;
  if (d.trajectory.has_value()) {
    j["trajectory"] = *d.trajectory;
  }
}
void from_json(const json &j, EdgeState &d) {
  if (j.contains("edgeDescription")) {
    d.edgeDescription = j.at("edgeDescription");
  }
  d.edgeId = j.at("edgeId");
  d.released = j.at("released");
  d.sequenceId = j.at("sequenceId");
  if (j.contains("trajectory")) {
    d.trajectory = j.at("trajectory");
  }
}

void to_json(json &j, const NodeState &d) {
  if (d.nodeDescription.has_value()) {
    j["nodeDescription"] = *d.nodeDescription;
  }
  j["nodeId"] = d.nodeId;
  j["released"] = d.released;
  j["sequenceId"] = d.sequenceId;
  if (d.nodePosition.has_value()) {
    j["nodePosition"] = *d.nodePosition;
  }
}
void from_json(const json &j, NodeState &d) {
  if (j.contains("nodeDescription")) {
    d.nodeDescription = j.at("nodeDescription");
  }
  d.nodeId = j.at("nodeId");
  d.released = j.at("released");
  d.sequenceId = j.at("sequenceId");
  if (j.contains("nodePosition")) {
    d.nodePosition = j.at("nodePosition");
  }
}

void to_json(json &j, const ErrorReference &d) {
  j["referenceKey"] = d.referenceKey;
  j["referenceValue"] = d.referenceValue;
}
void from_json(const json &j, ErrorReference &d) {
  d.referenceKey = j.at("referenceKey");
  d.referenceValue = j.at("referenceValue");
}

void to_json(json &j, const Error &d) {
  if (d.errorDescription.has_value()) {
    j["errorDescription"] = *d.errorDescription;
  }
  j["errorLevel"] = d.errorLevel;
  j["errorType"] = d.errorType;
  if (d.errorReferences.has_value()) {
    j["errorReferences"] = *d.errorReferences;
  }
}

void to_json(json &j, const ErrorLevel &d) {
  switch (d) {
    case vda5050pp::ErrorLevel::WARNING:
      j = "WARNING";
      break;
    case vda5050pp::ErrorLevel::FATAL:
      j = "FATAL";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, ErrorLevel &d) {
  auto str = j.get<std::string>();
  if (str == "WARNING") {
    d = vda5050pp::ErrorLevel::WARNING;
  } else if (str == "FATAL") {
    d = vda5050pp::ErrorLevel::FATAL;
  }
}

void from_json(const json &j, Error &d) {
  if (j.contains("errorDescription")) {
    d.errorDescription = j.at("errorDescription");
  }
  d.errorLevel = j.at("errorLevel");
  d.errorType = j.at("errorType");
  if (j.contains("errorReferences")) {
    std::vector<vda5050pp::ErrorReference> val = j.at("errorReferences");
    d.errorReferences = val;
  }
}

void to_json(json &j, const InfoReference &d) {
  j["referenceKey"] = d.referenceKey;
  j["referenceValue"] = d.referenceValue;
}
void from_json(const json &j, InfoReference &d) {
  d.referenceKey = j.at("referenceKey");
  d.referenceValue = j.at("referenceValue");
}

void to_json(json &j, const InfoLevel &d) {
  switch (d) {
    case vda5050pp::InfoLevel::DEBUG:
      j = "DEBUG";
      break;
    case vda5050pp::InfoLevel::INFO:
      j = "INFO";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, InfoLevel &d) {
  auto str = j.get<std::string>();
  if (str == "DEBUG") {
    d = vda5050pp::InfoLevel::DEBUG;
  } else if (str == "INFO") {
    d = vda5050pp::InfoLevel::INFO;
  }
}

void to_json(json &j, const Info &d) {
  if (d.infoDescription.has_value()) {
    j["infoDescription"] = *d.infoDescription;
  }
  j["infoLevel"] = d.infoLevel;
  j["infoType"] = d.infoType;
  if (d.infoReferences.has_value()) {
    j["infoReferences"] = *d.infoReferences;
  }
}
void from_json(const json &j, Info &d) {
  if (j.contains("infoDescription")) {
    d.infoDescription = j.at("infoDescription");
  }
  d.infoLevel = j.at("infoLevel");
  d.infoType = j.at("infoType");
  if (j.contains("infoReferences")) {
    std::vector<vda5050pp::InfoReference> val = j.at("infoReferences");
    d.infoReferences = val;
  }
}

void to_json(json &j, const BoundingBoxReference &d) {
  j["x"] = d.x;
  j["y"] = d.y;
  j["z"] = d.z;
  if (d.theta.has_value()) {
    j["theta"] = *d.theta;
  }
}
void from_json(const json &j, BoundingBoxReference &d) {
  d.x = j.at("x");
  d.y = j.at("y");
  d.z = j.at("z");
  if (j.contains("theta")) {
    d.theta = j.at("theta");
  }
}

void to_json(json &j, const LoadDimensions &d) {
  if (d.height.has_value()) {
    j["height"] = *d.height;
  }
  j["width"] = d.width;
  j["length"] = d.length;
}
void from_json(const json &j, LoadDimensions &d) {
  if (j.contains("height")) {
    d.height = j.at("height");
  }
  d.width = j.at("width");
  d.length = j.at("length");
}

void to_json(json &j, const Load &d) {
  if (d.boundingBoxReference.has_value()) {
    j["boundingBoxReference"] = *d.boundingBoxReference;
  }
  if (d.loadDimensions.has_value()) {
    j["loadDimensions"] = *d.loadDimensions;
  }
  if (d.loadId.has_value()) {
    j["loadId"] = *d.loadId;
  }
  if (d.loadPosition.has_value()) {
    j["loadPosition"] = *d.loadPosition;
  }
  if (d.loadType.has_value()) {
    j["loadType"] = *d.loadType;
  }
  if (d.weight.has_value()) {
    j["weight"] = *d.weight;
  }
}
void from_json(const json &j, Load &d) {
  if (j.contains("boundingBoxReference")) {
    d.boundingBoxReference = j.at("boundingBoxReference");
  }
  if (j.contains("loadDimensions")) {
    d.loadDimensions = j.at("loadDimensions");
  }
  if (j.contains("loadId")) {
    d.loadId = j.at("loadId");
  }
  if (j.contains("loadPosition")) {
    d.loadPosition = j.at("loadPosition");
  }
  if (j.contains("loadType")) {
    d.loadType = j.at("loadType");
  }
  if (j.contains("weight")) {
    d.weight = j.at("weight");
  }
}

void to_json(json &j, const SafetyState &d) {
  j["eStop"] = d.eStop;
  j["fieldViolation"] = d.fieldViolation;
}
void from_json(const json &j, SafetyState &d) {
  d.eStop = j.at("eStop");
  d.fieldViolation = j.at("fieldViolation");
}

void to_json(json &j, const State &d) {
  to_json(j, d.header);
  j["actionStates"] = d.actionStates;
  if (d.agvPosition.has_value()) {
    j["agvPosition"] = *d.agvPosition;
  }
  j["batteryState"] = d.batteryState;
  if (d.distanceSinceLastNode.has_value()) {
    j["distanceSinceLastNode"] = *d.distanceSinceLastNode;
  }
  j["driving"] = d.driving;
  j["edgeStates"] = d.edgeStates;
  j["errors"] = d.errors;
  j["informations"] = d.informations;
  j["lastNodeId"] = d.lastNodeId;
  j["lastNodeSequenceId"] = d.lastNodeSequenceId;
  if (d.loads.has_value()) {
    j["loads"] = *d.loads;
  }
  if (d.newBaseRequested.has_value()) {
    j["newBaseRequested"] = *d.newBaseRequested;
  }
  j["nodeStates"] = d.nodeStates;
  j["operatingMode"] = d.operatingMode;
  j["orderId"] = d.orderId;
  j["orderUpdateId"] = d.orderUpdateId;
  j["paused"] = d.paused;
  j["safetyState"] = d.safetyState;
  if (d.velocity.has_value()) {
    j["velocity"] = *d.velocity;
  }
  if (d.zoneSetId.has_value()) {
    j["zoneSetId"] = *d.zoneSetId;
  }
}
void from_json(const json &j, State &d) {
  from_json(j, d.header);
  d.actionStates = j.at("actionStates").get<std::vector<ActionState>>();
  if (j.contains("agvPosition")) {
    d.agvPosition = j.at("agvPosition");
  }
  d.batteryState = j.at("batteryState");
  if (j.contains("distanceSinceLastNode")) {
    d.distanceSinceLastNode = j.at("distanceSinceLastNode");
  }
  d.driving = j.at("driving");
  d.edgeStates = j.at("edgeStates").get<std::vector<EdgeState>>();
  d.errors = j.at("errors").get<std::vector<Error>>();
  d.informations = j.at("informations").get<std::vector<Info>>();
  d.lastNodeId = j.at("lastNodeId");
  d.lastNodeSequenceId = j.at("lastNodeSequenceId");
  if (j.contains("loads")) {
    d.loads = j.at("loads").get<std::vector<Load>>();
  }
  if (j.contains("newBaseRequested")) {
    d.newBaseRequested = j.at("newBaseRequested");
  }
  d.nodeStates = j.at("nodeStates").get<std::vector<NodeState>>();
  d.operatingMode = j.at("operatingMode");
  d.orderId = j.at("orderId");
  d.orderUpdateId = j.at("orderUpdateId");
  d.paused = j.at("paused");
  d.safetyState = j.at("safetyState");
  if (j.contains("velocity")) {
    d.velocity = j.at("velocity");
  }
  if (j.contains("zoneSetId")) {
    d.zoneSetId = j.at("zoneSetId");
  }
}

void to_json(json &j, const EStop &d) {
  switch (d) {
    case vda5050pp::EStop::AUTO_ACK:
      j = "AUTO_ACK";
      break;
    case vda5050pp::EStop::MANUAL:
      j = "MANUAL";
      break;
    case vda5050pp::EStop::REMOTE:
      j = "REMOTE";
      break;
    case vda5050pp::EStop::NONE:
      j = "NONE";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, EStop &d) {
  auto str = j.get<std::string>();
  if (str == "AUTO_ACK") {
    d = vda5050pp::EStop::AUTO_ACK;
  } else if (str == "MANUAL") {
    d = vda5050pp::EStop::MANUAL;
  } else if (str == "REMOTE") {
    d = vda5050pp::EStop::REMOTE;
  } else if (str == "NONE") {
    d = vda5050pp::EStop::NONE;
  }
}

void to_json(json &j, const OperatingMode &d) {
  switch (d) {
    case vda5050pp::OperatingMode::AUTOMATIC:
      j = "AUTOMATIC";
      break;
    case vda5050pp::OperatingMode::MANUAL:
      j = "MANUAL";
      break;
    case vda5050pp::OperatingMode::SEMI_AUTOMATIC:
      j = "SEMI_AUTOMATIC";
      break;
    case vda5050pp::OperatingMode::SERVICE:
      j = "SERVICE";
      break;
    case vda5050pp::OperatingMode::TEACHIN:
      j = "TEACHIN";
      break;
    default:
      j = "UNKNOWN";
      break;
  }
}
void from_json(const json &j, OperatingMode &d) {
  auto str = j.get<std::string>();
  if (str == "AUTOMATIC") {
    d = vda5050pp::OperatingMode::AUTOMATIC;
  } else if (str == "MANUAL") {
    d = vda5050pp::OperatingMode::MANUAL;
  } else if (str == "SEMI_AUTOMATIC") {
    d = vda5050pp::OperatingMode::SEMI_AUTOMATIC;
  } else if (str == "SERVICE") {
    d = vda5050pp::OperatingMode::SERVICE;
  } else if (str == "TEACHIN") {
    d = vda5050pp::OperatingMode::TEACHIN;
  }
}

}  // namespace vda5050pp