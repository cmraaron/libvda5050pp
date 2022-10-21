// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
//

#ifndef EXTRA_MQTT_CONNECTOR_INCLUDE_VDA5050_2B_2B_EXTRA_MQTT_CONNECTOR
#define EXTRA_MQTT_CONNECTOR_INCLUDE_VDA5050_2B_2B_EXTRA_MQTT_CONNECTOR

#include <mqtt/async_client.h>
#include <vda5050++/interface_agv/agv_description/agv_description.h>
#include <vda5050++/interface_mc/connector.h>

#include <atomic>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <string_view>

namespace vda5050pp::extra {

///
///\brief An Mqtt implementation of the vda5050pp::interface_mc::Connector
///
class MqttConnector final : public vda5050pp::interface_mc::Connector,
                            public mqtt::callback,
                            public mqtt::iaction_listener {
private:
  mqtt::async_client mqtt_client_;
  mqtt::connect_options connect_opts_;
  std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer_;
  std::string connection_topic_;
  std::string instant_actions_topic_;
  std::string order_topic_;
  std::string state_topic_;
  std::string visualization_topic_;
  vda5050pp::Header header_template_;
  std::atomic_int header_id_counter_ = 1;

  std::map<mqtt::delivery_token_ptr, mqtt::message_ptr> pending_deliveries_;

  const int k_qos = 0;
  bool shutdown_ = false;

  void reconnect();

public:
  class NotConnectedError : public std::logic_error {
  public:
    NotConnectedError() : std::logic_error("No active MQTT connection") {}
  };

  ///
  ///\brief Configuration struct for the MqttConnector
  ///
  struct MqttOptions {
    ///\brief username used for authentication
    std::optional<std::string> username;
    ///\brief password used for authentication
    std::optional<std::string> password;
    ///\brief overwrites <version> in all topics /<iface>/<version>/<manufacturer>/<sn>
    std::optional<std::string> version_overwrite;
    ///\brief sets the server address (use "ssl://<address>:<port>" or "tcp://<address>:<port>")
    std::string server;
    ///\brief sets <iface> in all topics /<iface>/<version>/<manufacturer>/<sn>
    std::string interface;
    ///\brief enable ssl certificate check
    bool enable_cert_check = true;
    ///\brief enable ssl
    bool use_ssl = true;
  };

  ///
  ///\brief Construct a new Mqtt Connector object
  ///
  ///\param desc agv_description to use (for serial_number and manufacturer)
  ///\param opts the mqtt connection options to use
  ///
  MqttConnector(const vda5050pp::interface_agv::agv_description::AGVDescription &desc,
                const MqttOptions &opts);

  ~MqttConnector();

  /**
   * This method is invoked when an action fails.
   * @param asyncActionToken
   */
  void on_failure(const mqtt::token &asyncActionToken) override;
  /**
   * This method is invoked when an action has completed successfully.
   * @param asyncActionToken
   */
  void on_success(const mqtt::token &asyncActionToken) override;

  /**
   * This method is called when the client is connected.
   * Note that, in response to an initial connect(), the token from the
   * connect call is also signaled with an on_success(). That occurs just
   * before this is called.
   */
  void connected(const std::string & /*cause*/) override;
  /**
   * This method is called when the connection to the server is lost.
   */
  void connection_lost(const std::string & /*cause*/) override;
  /**
   * This method is called when a message arrives from the server.
   */
  void message_arrived(mqtt::const_message_ptr /*msg*/) override;
  /**
   * Called when delivery for a message has been completed, and all
   * acknowledgments have been received.
   */
  void delivery_complete(mqtt::delivery_token_ptr /*tok*/) override;

  ///
  ///\brief Set the consumer for the ingoing messages
  ///
  ///\param consumer the consumer
  ///
  void setConsumer(std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(
      true) override;

  ///\brief Queue a connection message for sending
  void queueConnection(const vda5050pp::Connection &connection) noexcept(false) override;

  /// \brief Queue a State message for sending
  void queueState(const vda5050pp::State &state) noexcept(false) override;

  /// \brief Queue a Visualization message for sending
  void queueVisualization(const vda5050pp::Visualization &visualization) noexcept(false) override;

  ///
  ///\brief Start the connection
  ///
  /// This can be used to manually initiate a connection
  ///
  void connect() noexcept(false) override;

  ///
  ///\brief End the current connection, send offline msg
  ///
  /// This can be used to manually end the active connection
  ///
  void disconnect() noexcept(false) override;
};

}  // namespace vda5050pp::extra

#endif /* EXTRA_MQTT_CONNECTOR_INCLUDE_VDA5050_2B_2B_EXTRA_MQTT_CONNECTOR */
