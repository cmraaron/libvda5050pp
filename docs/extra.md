# libVDA5050++ Extra Components

The library includes some extra components of common code
used alongside the library (e.g. an MQTT connector).

## Enabling

Section [Build: Configuring the build](/build#configuring-the-build) covers
the process of configuring the used components.

## mqtt_connector

The `mqtt_connector` component makes the `mqtt_connector` CMake target
available. (It requires the `json_model` target)

It provides the `vda5050pp::extra::MqttConnector` class, which can be used to
make the library communicate via MQTT.

### Configuration

The `vda5050pp::extra::MqttConnector::MqttOptions` struct is used to configure the
mqtt connection.

```c++
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
```

### Example

The following snippet shows the initialization of the `MqttConnector`:

```c++
#include <memory>
#include <vda5050++/extra/mqtt_connector.h>

int main() {

  // ...
  // configuration of agv_description ect.

  vda5050pp::extra::MqttConnector::MqttOptions mqtt_options;
  mqtt_options.username = "user";
  mqtt_options.password = "password";
  mqtt_options.server = "ssl://vda5050-broker.example.net";


  auto mqtt_connector_ptr = std::make_shared<vda5050pp::extra::MqttConnector>(agv_description, mqtt_options);


  // ...
  // further library initialization

}

```

## json_model

The json_model component makes the `json_model`
build target available.

This is mainly used by MqttConnector, but can be freely used 
inside of the user-code, if necessary.

It provides overloaded `to_json` and `from_json` methods
(see [nlohmann::json](https://github.com/nlohmann/json#examples))
for all data structures in `vda5050++/model`.

### Example

The following snipped serializes a `vda5050pp::AGVPosition` object
and dumps it to stdout.

```c++
#include <iostream>
#include <vda5050++/extra/json_model.h>
#include <vda5050++/model/AGVPosition.h>


int main() {
  vda5050pp::AGVPosition pos;
  //...

  json pos_json = pos;

  std::cout << pos_json.dump(2) << std::endl;
}
```

For further details on de-/serialization, see [nlohmann::json](https://github.com/nlohmann/json#examples).

## logger_utils

The logger_utils component makes the `logger_utils`
build target available.

They provide some logger classes, that can be plugged together or can directly
be used by the library.

### `vda5050pp::extra::ConsoleLogger`

This is a ready to use console logger.

```c++
#include <vda5050++/extra/console_logger.h>

int main() {

  //...

  auto logger_ptr = std::make_shared<vda5050pp::extra::ConsoleLogger>("optional line prefix");

  //...

}
```

### `vda5050pp::extra::FileLogger`

This is a ready to use file logger.

```c++
#include <vda5050++/extra/file_logger.h>

int main() {

  //...

  auto logger_ptr = std::make_shared<vda5050pp::extra::FileLogger>("file path");

  //...

}
```


### `vda5050pp::extra::MultiplexLogger`

This logger can be used to fan out log calls to multiple loggers.


```c++
#include <vda5050++/extra/multiplex_logger.h>

int main() {

  //...

  std::unique_ptr<vda5050pp::interface_agv::Logger> logger_a = /*...*/;
  std::unique_ptr<vda5050pp::interface_agv::Logger> logger_b = /*...*/;
  std::unique_ptr<vda5050pp::interface_agv::Logger> logger_c = /*...*/;

  // Loggers have to be explicitly inserted, because std::initializer_list does not support move semantics
  std::set<std::unique_ptr<vda5050pp::interface_agv::Logger>> logger_sinks;
  logger_sinks.insert(std::move(logger_a));
  logger_sinks.insert(std::move(logger_b));
  logger_sinks.insert(std::move(logger_c));

  auto logger_ptr = std::make_shared<vda5050pp::extra::MultiplexLogger>(std::move(logger_sinks));

  //...

}
```

### `vda5050pp::extra::PrettyLogger`

This logger can be used to make log strings _pretty_, as the output of
the console logger is.


```c++
#include <vda5050++/extra/pretty_logger.h>

int main() {

  //...

  std::unique_ptr<vda5050pp::interface_agv::Logger> logger_sink = /*...*/;

  auto logger_ptr = std::make_shared<vda5050pp::extra::PrettyLogger>(std::move(logger_sink),
                                                                     "line prefix");

  //...

}
```