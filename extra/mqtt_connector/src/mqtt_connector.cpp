// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
//

#include "vda5050++/extra/mqtt_connector.h"

#include <sstream>
#include <thread>

#include "vda5050++/core/common/formatting.h"
#include "vda5050++/core/version.h"
#include "vda5050++/extra/json_model.h"
#include "vda5050++/interface_agv/logger.h"

using namespace vda5050pp::extra;
using namespace std::chrono_literals;
using namespace vda5050pp::core::common;

MqttConnector::MqttConnector(const vda5050pp::interface_agv::agv_description::AGVDescription &desc,
                             const MqttOptions &opts)
    : mqtt_client_(opts.server, desc.agv_id) {
  this->mqtt_client_.set_callback(*this);
  this->connect_opts_.set_mqtt_version(4);
  this->connect_opts_.set_clean_session(false);
  this->connect_opts_.set_user_name(opts.username.value_or(""));
  this->connect_opts_.set_password(opts.password.value_or(""));
  if (opts.use_ssl) {
    mqtt::ssl_options ssl;
    ssl.set_verify(opts.enable_cert_check);
    mqtt::ssl_options::error_handler handler = [](const std::string &msg) {
      vda5050pp::interface_agv::Logger::getCurrentLogger()->logError(
          format("MqttConnector (ssl_error): {}", msg));
    };
    ssl.set_error_handler(handler);
    ssl.set_enable_server_cert_auth(opts.enable_cert_check);
    this->connect_opts_.set_ssl(ssl);
  }

  auto mkTopic = [&opts, &desc](auto &subtopic) -> std::string {
    std::stringstream topic;
    topic << opts.interface << '/' << opts.version_overwrite.value_or("v1") << '/'
          << desc.manufacturer << '/' << desc.serial_number << '/' << subtopic;
    return topic.str();
  };

  this->order_topic_ = mkTopic("order");
  this->connection_topic_ = mkTopic("connection");
  this->visualization_topic_ = mkTopic("visualization");
  this->instant_actions_topic_ = mkTopic("instantActions");
  this->state_topic_ = mkTopic("state");

  this->header_template_.manufacturer = desc.manufacturer;
  this->header_template_.serialNumber = desc.serial_number;
  this->header_template_.version = vda5050pp::core::version::current;

  this->shutdown_ = false;
}

MqttConnector::~MqttConnector() {
  this->shutdown_ = true;
  this->disconnect();
}

void MqttConnector::on_failure(const mqtt::token &) {
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logInfo("MQTT: Failure");
  this->reconnect();
}

void MqttConnector::on_success(const mqtt::token &) {
  // reconnected
}

void MqttConnector::connected(const std::string &) {
  this->mqtt_client_.subscribe(this->order_topic_, this->k_qos);
  this->mqtt_client_.subscribe(this->instant_actions_topic_, this->k_qos);

  vda5050pp::Connection online_msg;
  online_msg.header = this->header_template_;
  online_msg.header.headerId = this->header_id_counter_++;
  online_msg.header.timestamp = std::chrono::system_clock::now();
  online_msg.connectionState = vda5050pp::ConnectionState::ONLINE;

  this->queueConnection(online_msg);

  vda5050pp::interface_agv::Logger::getCurrentLogger()->logInfo("MqttConnector: connected");
}

void MqttConnector::connection_lost(const std::string &cause) {
  using namespace std::chrono_literals;

  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logDebug(format("MQTT: connection lost ({})", cause));
  this->reconnect();
}

void MqttConnector::message_arrived(mqtt::const_message_ptr msg) {
  auto consumer = this->consumer_.lock();

  if (consumer == nullptr) {
    return;
  }

  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();

  try {
    auto j = json::parse(msg->get_payload());
    if (msg->get_topic() == this->order_topic_) {
      consumer->receivedOrder(j);
    } else if (msg->get_topic() == this->instant_actions_topic_) {
      consumer->receivedInstantActions(j);
    } else {
      logger->logWarn(format("Received MQTT message on unknown topic \"{}\"", msg->get_topic()));
    }
  } catch (const json::exception &e) {
    logger->logError(
        format("MQTT deserialization exception: {} on topic \"{}\"", e.what(), msg->get_topic()));
  }

  logger->logDebug(format("MQTT received @{}", msg->get_topic()));
}

void MqttConnector::delivery_complete(mqtt::delivery_token_ptr tok) {
  this->pending_deliveries_.erase(tok);
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logDebug(format("MQTT: delivered message id={}", tok->get_message_id()));
}

void MqttConnector::reconnect() {
  using namespace std::chrono_literals;

  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logInfo("MQTT: reconnecting in 2s...");
  std::this_thread::sleep_for(2s);

  try {
    this->mqtt_client_.reconnect();
  } catch (const mqtt::exception &e) {
    logger->logWarn(format("MQTT: reconnect() exception: {}", e.what()));
  }
}

void MqttConnector::setConsumer(
    std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) {
  this->consumer_ = consumer;
}

void MqttConnector::queueConnection(const vda5050pp::Connection &connection) noexcept(false) {
  if (!this->mqtt_client_.is_connected()) {
    throw NotConnectedError();
  }

  auto msg = std::make_shared<mqtt::message>();
  msg->set_qos(this->k_qos);
  msg->set_topic(this->connection_topic_);
  msg->set_retained(true);

  json j = connection;
  msg->set_payload(j.dump());

  auto tok = this->mqtt_client_.publish(msg);
  this->pending_deliveries_[tok] = msg;
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logDebug(
      format("MQTT: queued message id={} on topic {}", tok->get_message_id(), msg->get_topic()));
}

void MqttConnector::queueState(const vda5050pp::State &state) noexcept(false) {
  if (!this->mqtt_client_.is_connected()) {
    throw NotConnectedError();
  }

  auto msg = std::make_shared<mqtt::message>();
  msg->set_qos(this->k_qos);
  msg->set_topic(this->state_topic_);

  json j = state;
  msg->set_payload(j.dump());

  auto tok = this->mqtt_client_.publish(msg);
  this->pending_deliveries_[tok] = msg;
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logDebug(
      format("MQTT: queued message id={} on topic {}", tok->get_message_id(), msg->get_topic()));
}

void MqttConnector::queueVisualization(const vda5050pp::Visualization &visualization) noexcept(
    false) {
  if (!this->mqtt_client_.is_connected()) {
    throw NotConnectedError();
  }

  auto msg = std::make_shared<mqtt::message>();
  msg->set_qos(this->k_qos);
  msg->set_topic(this->visualization_topic_);

  json j = visualization;
  msg->set_payload(j.dump());

  auto tok = this->mqtt_client_.publish(msg);
  this->pending_deliveries_[tok] = msg;
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();
  logger->logDebug(
      format("MQTT: queued message id={} on topic {}", tok->get_message_id(), msg->get_topic()));
}

void MqttConnector::connect() noexcept(false) {
  auto logger = vda5050pp::interface_agv::Logger::getCurrentLogger();

  vda5050pp::Connection will_msg;
  will_msg.header = this->header_template_;
  will_msg.header.timestamp = std::chrono::system_clock::now();
  will_msg.header.headerId = this->header_id_counter_ + 1;  // after online msg
  will_msg.connectionState = vda5050pp::ConnectionState::CONNECTIONBROKEN;

  mqtt::will_options will;
  will.set_topic(this->connection_topic_);
  will.set_retained(true);
  will.set_qos(this->k_qos);
  will.set_payload(json(will_msg).dump());

  this->connect_opts_.set_will(std::move(will));

  try {
    auto tok = this->mqtt_client_.connect(this->connect_opts_, nullptr, *this);
    if (tok == nullptr) {
      logger->logInfo(format("MQTT: connect() no token returned"));
      return;
    }

    if (!tok->wait_for(15000)) {
      logger->logInfo(format("MQTT: connect() timed out"));
    }
  } catch (const mqtt::exception &e) {
    logger->logError(format("MQTT: connect() exception: {}", e.what()));
  }
}

void MqttConnector::disconnect() noexcept(false) {
  if (!this->mqtt_client_.is_connected()) {
    throw NotConnectedError();
  }

  vda5050pp::Connection offline_msg;
  offline_msg.header = this->header_template_;
  offline_msg.header.timestamp = std::chrono::system_clock::now();
  offline_msg.header.headerId = this->header_id_counter_++;
  offline_msg.connectionState = vda5050pp::ConnectionState::OFFLINE;

  auto msg = std::make_shared<mqtt::message>();
  msg->set_qos(this->k_qos);
  msg->set_topic(this->connection_topic_);
  msg->set_retained(true);

  json j = offline_msg;
  msg->set_payload(j.dump());

  auto tok = this->mqtt_client_.publish(msg);
  tok->wait_for(5s);
  this->mqtt_client_.disconnect();
}