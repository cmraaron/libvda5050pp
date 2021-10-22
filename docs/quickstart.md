# libVDA5050++ quick-start guide

## Table of Content

- [libVDA5050++ quick-start guide](#libvda5050-quick-start-guide)
  - [Table of Content](#table-of-content)
  - [0. Requirements](#0-requirements)
  - [1. Overview](#1-overview)
  - [2. Implementing the AGV Interface](#2-implementing-the-agv-interface)
    - [VDA5050 Action Handler](#vda5050-action-handler)
    - [Navigation](#navigation)
      - [Implementing the `StepBasedNavigationHandler`](#implementing-the-stepbasednavigationhandler)
      - [Implementing the `ContinuousNavigationHandler`](#implementing-the-continuousnavigationhandler)
      - [Implementing the `PauseResumeHandler`](#implementing-the-pauseresumehandler)
  - [Implementing the MC Interface](#implementing-the-mc-interface)
    - [Implementing the `Connector`](#implementing-the-connector)
    - [Implementing the `ConnectorPassive`](#implementing-the-connectorpassive)
  - [3. Configuring the AGV](#3-configuring-the-agv)
  - [4. Running the Library](#4-running-the-library)

## 0. Requirements

This Guide assumes, that you have all necessary CMake targets
the library provides available.

If not, follow either of the two guides at [build](/build).

## 1. Overview

libVDA5050++ is a passive network implementing VDA5050 compliant behavior. It can be driven by implementing interface classes to feed the library with data. On one side `namespace vda5050pp::interface_agv` are the classes
that implement AGV behavior, such as actions and navigation. On the other side there are the `namespace vda5050pp::interface_mc` classes, which implement the message gathering (MQTT, ROS,...) to receive messages from the MC.

## 2. Implementing the AGV Interface

There are a few classes that need to be implemented by the user.
This section is a brief overview over the classes that need to be implemented and how they are used.

All Handlers are instantiated by the library, once 
they are needed. Each Handler is associated with a certain set VDA5050
objects, which describe the specific task of the Handler. The registration of
such handlers is described in section 3,4.

For a more detailed documentation of the Functions and Handler classes, refer
to the Doxygen documentation.

### VDA5050 Action Handler

To implement an action actuator the `vda5050pp::interface_agv::ActionHandler` must be overridden. This can look as follows:

```c++
#include <vda5050++/interface_agv/action_handler.h>

class MyActionHandler : public vda5050pp::interface_agv::ActionHandler {
public:
virtual ~MyActionHandler();

virtual void start(const vda5050pp::Action &action) override;

virtual void pause(const vda5050pp::Action &action) override;

virtual void resume(const vda5050pp::Action &action) override;

virtual void stop(const vda5050pp::Action &action) override;
};
```

As you can see there are four action specific member functions that need
to be overridden:

- start() - is called once the associated action needs to be started
- pause() - is called once the associated actions needs to be paused
- resume() - is called once the associated action needs to be resumed
- stop() - is called once the associated action has to stop (either fail or succeed)

NOTE: Each instance of `MyActionHandler` is associated with one and only one
`vda5050pp::Action`, hence an action specific state can be maintained
within the specialized Handler. The Action can also be obtained via `vda5050pp::interface_agv::ActionHandler::getAction`.

Implementation of the member functions might look the following:

_The action_controller is a member of `MyActionHandler` specific for the AGV_

```c++
virtual void start(const vda5050pp::Action &action) override {
this->action_controller.run(action.actionType);
auto success = this->action_controller.wait_for_start();
if (success) {
  // This is a protected function of vda5050pp::interface_agv::ActionHandler 
  // it needs to be called once the action is started.
  this->started();
} else {
  // These are protected functions ov vda5050pp::interface_agv::ActionHandler
  // addError can be called to add errors associated to this action
  this->addError(this->action_controller.getError(), vda5050pp::ErrorLevel::WARNING);
  // failed() has to be called once the action has failed
  // FAILED is a terminal state.
  this->failed();
}
}
```

As you might have noticed `start()` might block, if `action_controller.wait_for_start()` does not return immediately. When Handler functions
do not return immediately the library should be run multi threaded. This will
be explained in section 4.


```c++
virtual void pause(const vda5050pp::Action &action) override {
this->action_controller.pause()
auto success = this->action_controller.wait_for_pause();
if (success) {
  // This is a protected function of vda5050pp::interface_agv::ActionHandler 
  // it needs to be called once the action is paused.
  this->paused();
} else {
  // Handle error
  // SEE start() implementation
}
}
```

```c++
virtual void resume(const vda5050pp::Action &action) override {
this->action_controller.resume()
auto success = this->action_controller.wait_for_resume();
if (success) {
  // This is a protected function of vda5050pp::interface_agv::ActionHandler 
  // it needs to be called once the action is resumed.
  this->resumed();
} else {
  // Handle error
  // SEE start() implementation
}
}
```

```c++
virtual void stop(const vda5050pp::Action &action) override {
this->action_controller.stop();
auto state = this->action_controller.wait_for_stop();
// When stopping an action, it can either still finish (if it is almost done)
// or fail. The state needs to be reported via the protected member functions
// of vda5050pp::interface_agv::ActionHandler
switch (state) {
  case state::finished:
    this->finished(); // FINISHED is a terminal state
    break;
  case state::preempted:
    this->failed(); // FAILED is a terminal state
    break;
}
}
```

### Navigation

Currently there are two different kinds of Navigation Handlers:

- `StepBasedNavigationHandler` - for "line-guided" AGVs that only navigate to one node at the same time, then stop.
- `ContinuousNavigationHandler` - for AGVs that can handle multiple navigation nodes at the same time. (Allows a continuous driving behavior)

You can decide to implement the Handler that suites your AGV best.

#### Implementing the `StepBasedNavigationHandler`



```c++
#include <vda5050++/interface_agv/step_based_navigation_handler.h>

class MyNavigationHandler : public vda5050pp::interface_agv::StepBasedNavigationHandler {
public:
virtual ~MyNavigationHandler();

virtual void start(const std::optional<vda5050pp::Edge> &via_edge,
                    const vda5050pp::Node &goal_node) override;

virtual void pause() override;

virtual void resume() override;

virtual void stop() override;
};
```

As you can see there are four navigation specific member functions that need
similar to the ones of the `ActionHandler` to be overridden:

- start() - is called once the navigation to the associated node needs to be started
- pause() - is called once the navigation to the associated node needs to be paused
- resume() - is called once the navigation to the associated node needs to be resumed
- stop() - is called once the navigation to the associated node has to stop. The AGV can either reach the node if it cannot stop in between nodes, or stop in place (calling failed() or finished() respectively)

NOTE: Each instance of `MyNavigation` is associated with one and only one
`vda5050pp::Node` and `vda5050pp::Edge`, hence a goal specific state can be maintained
within the specialized Handler. The Node and Edge can also be obtained via `vda5050pp::interface_agv::ActionHandler::getGoalNode` and
`vda5050pp::interface_agv::ActionHandler::getViaEdge`.

Implementation of the member functions might look the following:

_The navigation_controller is a member of `MyNavigationHandler` specific for the AGV_

```c++
virtual void start(const std::optional<vda5050pp::Edge> &via_edge,
                    const vda5050pp::Node &goal_node) override{
if (via_edge.has_value()) {
  // There is an edge leading to the node (not the initial node)
  this->navigation_controller.setGoal(via_edge, goal_node)
} else {
  // The goal_node is the first node of the graph. It is trivially reachable
  this->navigation_controller.setGoal(goal_node);
}

// Handle feedback of the navigation
this->navigation_controller.feedback_cb([this](const auto &position, const auto &distance) {
  // Convert position representation
  vda5050pp::AGVPosition pos = position.toVDA5050();
  // Optionally set position data if available
  this->setPosition(pos);
  // Optionally set distance data if available
  this->updateDistanceSinceLastNode(distance);
});

// Status
auto success = this->navigation_controller.wait_for_active();
if (success) {
  // AGV is driving
  this->started();
} else {
  this->addError(this->navigation_controller.getError(), vda5050pp::ErrorLevel::WARNING);
  this->failed();
}
}
```

As above, you might have noticed `start()` might block, if `navigation_controller.wait_for_start()` does not return immediately. When Handler functions
do not return immediately the library should be run multi threaded. This will
be explained in section 4.

```c++
virtual void pause() override  {
this->navigation_controller.pause();
auto success = this->navigation_controller.wait_for_pause();

if (success) {
  this->paused();
} else {
  // Handle error, see above
}
}

```
```c++
virtual void resume() override {
this->navigation_controller.resume();
auto success = this->navigation_controller.wait_for_resume();

if (success) {
  this->resumed();
} else {
  // Handle error, see above
}
}
```

```c++
virtual void stop() override {
this->navigation_controller.stop();
auto status = this->navigation_controller.wait_for_stop();

// Either stop in place (fail) or drive to goal (finish)
switch (status) {
  case status::finished:
    this->finished();
    break;
  case status::preempted:
    this->failed();
    break;
}
}
```


#### Implementing the `ContinuousNavigationHandler`

```c++
#include <vda5050++/interface_agv/continuous_navigation_handler.h>

class MyNavigationHandler : public vda5050pp::interface_agv::ContinuousNavigationHandler {
public:
virtual void horizonUpdated(const std::list<vda5050pp::Node> &new_horizon_nodes,
                            const std::list<vda5050pp::Edge> &new_horizon_edges) override;

virtual void baseIncreased(const std::list<vda5050pp::Node> &base_append_nodes,
                            const std::list<vda5050pp::Edge> &base_append_edges) override;

virtual void start(const std::list<vda5050pp::Node> &base_nodes,
                    const std::list<vda5050pp::Edge> &base_edges) override;

virtual void pause() override;

virtual void resume() override;

virtual void stop() override;
};
```

This handler has a few different member functions, that need to be overridden:

- horizonUpdated - a new VDA5050 Horizon is available, the AGV can plan it's path but not
traverse the horizon
- baseIncreased - the AGV can traverse new nodes and edges. `base_append_nodes` contains
all new traversable nodes. `base_append_edges` contains all
new edges introduced by `base_append_nodes`.
- start - the AGV must start traversing all `base_nodes` in order.
`vda5050pp::interface_agv::ContinuousNavigationHandler::started` must be called
once the AGV is driving.
- pause - the AGV must pause navigation.
`vda5050pp::interface_agv::ContinuousNavigationHandler::paused` must be called once
the AGV paused. This function must also be called when the AGV pauses due to another reason.
- resume - the AGV must resume navigation.
`vda5050pp::interface_agv::ContinuousNavigationHandler::resumed` must be called once
the AGV resumed. This function must also be called when the AGV resumes due to another reason.
- stop - the AGV must stop navigation. It either navigates to the next node if it cannot stop in between
nodes or stops in place. Either `vda5050pp::interface_agv::ContinuousNavigationHandler::failed`
is called to indicate a stop in between nodes or the next node is set to reached to indicate
a stop on the next node.

The progress of traversed nodes can be reported with a few different protected member functions
of `vda5050pp::interface_agv::ContinuousNavigationHandler`:

- setNodeReached() - report the next reached node (must be the current goal node)
- evalPosition() - set new position data and tell the library to check if the new position
matches the deviation of the current goal node. If so setNodeReached is called automatically
and true is returned.

NOTE: Each `MyNavigationHandler` instance is associated with an uninterrupted subgraph of the order,
such that each instance can traverse the associated subgraph without encountering a SOFT or HARD blocking
action (no need to stop, besides blocking instant actions). If the order is extended, the currently active
`MyNavigationHandler` instance is notified about the new nodes via `MyNavigationHandler::baseIncreased`.

Implementation of the member functions might look the following:

_The navigation_controller is a member of `MyNavigationHandler` specific for the AGV_


```c++
virtual void horizonUpdated(const std::list<vda5050pp::Node> &new_horizon_nodes,
                          const std::list<vda5050pp::Edge> &new_horizon_edges) override {
// Optionally prepare the navigation stack for upcoming nodes
this->navigation_controller.prepare(new_horizon_nodes);
}
```
```c++
virtual void baseIncreased(const std::list<vda5050pp::Node> &base_append_nodes,
                          const std::list<vda5050pp::Edge> &base_append_edges) override {
this->navigation_controller.addGoals(base_append_nodes.begin(), base_append_nodes.end());
}
```

```c++
virtual void start(const std::list<vda5050pp::Node> &base_nodes,
                  const std::list<vda5050pp::Edge> &base_edges) override {
this->navigation_controller.addGoals(base_append_nodes);
this->navigation_controller.onStarted([this]{ this->started(); });
this->navigation_controller.onPaused([this]{ this->paused(); });
this->navigation_controller.onResumed([this]{ this->resumed(); });

// Optionally instead of goal_reached
this->navigation_controller.onFeedback([this](const auto &position) {
  // Process position without localization deviation.
  // Nodes will be automatically set as traversed and actions might be triggered.
  bool reached_node = this->evalPosition(position.x, position.y, position.theta, position.map);
});

// Optionally instead of position
this->navigation_controller.onFeedback([this](const auto &goal_reached) {
  this->setNodeReached(goal_reached.sequenceId);
});
}
```
```c++
virtual void pause() override {
this->navigation_controller.pause();
}
```
```c++
virtual void resume() override {
this->navigation_controller.resume();
}
```
```c++
virtual void stop() override {
this->navigation_controller.stop();
auto status = this->navigation_controller.wait_for_stop();

switch (status) {
  case status::PREEMPTED:
    this->failed();
    break;
  case status::FINISHED:
    // Assume next node was reached. Feedback cb in start() has notified the library
    // via setNodeReached. No further action needed.
    break;
}
}
```

#### Implementing the `PauseResumeHandler`

The last AGV interface class to implement is the `PauseResumeHandler`
it's purpose is to integrate the user-code and the AGV into the pause and 
resume process. The library creates a `PauseResumeHandler` each time a
pause or resume request was received. The `PauseResumeHandler` then decides
if the Pause or Resume is possible (e.g. hardware switch active).
Inside of the handler member functions you can either notify your action-controller
and navigation controller directly or let the library notify each active
action and navigation handler about the pause or resume request.

A Class sketch might might look like this:
```c++
#include <vda5050++/interface_agv/pause_resume_handler.h>

class MyPauseResumeHandler : public vda5050pp::interface_agv::PauseResumeHandler {
public:
virtual ~MyPauseResumeHandler();

virtual void doPause() override;

virtual void doResume() override;
};
```

- doPause() - is called once a pause request was received. Accepting the pause request must be notified with `vda5050pp::interface_agv::PauseResumeHandler::finished`.
If pausing is not possible, `vda5050pp::interface_agv::PauseResumeHandler::failed`
must be called.
- doResume() - is called once a resume request was received. Accepting the resume request must be notified with `vda5050pp::interface_agv::PauseResumeHandler::finished`.
If resuming is not possible, `vda5050pp::interface_agv::PauseResumeHandler::failed` must be called.

A possible implementation might look like this:

_The agv_controller is a member of `MyPauseResumeHandler` specific for the AGV_

```c++
virtual void doPause() override {
if (this->agv_controller.can_pause()) {
  // Either
  this->agv_controller.pause_all_controllers();
  // Or
  this->notifyActions(); // Calls MyActionHandler::pause() for each action
  this->notifyNavigation(); // Calls MyNavigationHandler::pause() for the currently active handler

  // This can be called, as soon, as it is certain that the AGV can pause
  this->finished();
} else {
  this->addError(...); // Optional
  this->failed();
}
}
```
```c++
virtual void doResume() override {
if (this->agv_controller.can_resume()) {
  // Either
  this->agv_controller.pause_all_controllers();
  // Or
  this->notifyActions(); // Calls MyActionHandler::pause() for each action
  this->notifyNavigation(); // Calls MyNavigationHandler::pause() for the currently active handler

  // This can be called, as soon, as it is certain, that the AGV can resume
  this->finished();
} else {
  this->addError(...); // Optional
  this->failed();
}
}
```

## Implementing the MC Interface

There is currently one implemented connector (MQTT). You can skip this
section if you intend to it. Otherwise when you intend to use another
messaging middleware, then you will need to implement your own connector.

There are two types of connectors:

- `vda5050pp::interface_mc::Connector` - connector for a multi-threaded environment
- `vda5050pp::interface_mc::ConnectorPassive` - thread-less connector based on message polling

### Implementing the `Connector`

This will most likely be the connector of your choice. A class sketch might look like this:

```c++
#include <vda5050++/interface_mc/connector.h>

class MyConnector : public vda5050pp::interface_mc::Connector {
public:
virtual ~MyConnector();

virtual void setConsumer(
    std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) override;

virtual void queueConnection(const vda5050pp::Connection &connection) noexcept(false) override;

virtual void queueState(const vda5050pp::State &state) noexcept(false) override;

virtual void queueVisualization(const vda5050pp::Visualization &visualization) noexcept(
    false) override;

virtual void connect() noexcept(false) override;

virtual void disconnect() noexcept(false) override;

virtual void disconnect(const vda5050pp::Connection &connection) noexcept(false) override;
};
```

- setConsumer() - the library registers itself within the connector as a `vda5050pp::interface_mc::MessageConsumer`. The connector dumps all received messages
to the consumer.
- queueConnection() - send or enqueue a message in the connection topic
- queueState() - send or enqueue a message in the state topic
- queueVisualization() - send or enqueue a message in the visualization topic
- connect() - connect to the messaging layer
- disconnect() - disconnect from the messaging layer (with a last connection message)

_The message_middleware is a member of `MyConnector`_

```c++
virtual void setConsumer(
  std::weak_ptr<vda5050pp::interface_mc::MessageConsumer> consumer) noexcept(true) override {

// Set receiver for the connection topic
this->message_middleware.receive_cb("connection", [consumer](const auto &conn) {
  auto ptr = consumer.lock();
  if (ptr != nullptr) {
    auto msg = toVDA5050pp(conn); // covert
    ptr->receivedConnection(msg); // pass to consumer
  } else {
    // library was shut down
  }
});

// Set receiver for the instantAction topic
this->message_middleware.receive_cb("instantAction", [consumer](const auto &ia) {
  auto ptr = consumer.lock();
  if (ptr != nullptr) {
    auto msg = toVDA5050pp(ia); // convert 
    ptr->receivedInstantActions(msg); // pass to consumer
  } else {
    // library was shut down
  }
});

// Set receiver for the order topic
this->message_middleware.receive_cb("order", [consumer](const auto &order) {
  auto ptr = consumer.lock();
  if (ptr != nullptr) {
    auto msg = toVDA5050pp(order); // convert
    ptr->receivedOrder(msg); // pass to consumer
  } else {
    // library was shut down
  }
});
}
```
```c++
virtual void queueConnection(const vda5050pp::Connection &connection) noexcept(false) override {
auto msg = fromVDA5050pp(connection); // convert format
this->message_middleware.send("connection", msg); // send on the connection topic
}
```
```c++
virtual void queueState(const vda5050pp::State &state) noexcept(false) override {
auto msg = fromVDA5050pp(state); // convert format
this->message_middleware.send("state", msg); // send on the state topic
}
```
```c++
virtual void queueVisualization(const vda5050pp::Visualization &visualization) noexcept(
  false) override {
auto msg = fromVDA5050pp(visualization); // convert format
this->message_middleware.send("visualization", msg); // send on the visualization topic
}
```
```c++
virtual void connect() noexcept(false) override {
this->message_middleware.connect();
}
```
```c++
virtual void disconnect() noexcept(false) override {
this->message_middleware.disconnect();
}
```
```c++
virtual void disconnect(const vda5050pp::Connection &connection) noexcept(false) override {
this->queueConnection(connection);
this->message_middleware.flush(); // ensure message was sent
this->disconnect();
}
```

### Implementing the `ConnectorPassive`

The `ConnectorPassive` inherits all member functions from the `Connector`.
There is only one new member function `vda5050pp::interface_mc::ConnectorPassive::spinOnce`.
This function is called each time the library spins. Inside this functions
messages can be polled and passed to the message connector.

## 3. Configuring the AGV

The library needs an AGV configuration, to handle message processing.
There is a main configuration structure `vda5050pp::interface_agv::agv_description::AGVDescription`
This section only covers the used fields of the structure. All other fields might be used in later versions.
There is more information about the classes and structures inside of the doxygen documentation.

The AGV configuration contains declarations for all actions the AGV can execute for
automatic message verification. The following snippet explains the configuration based on
an example.

```c++
#include <vda5050++/interface_agv/agv_description/agv_description.h>

// ...

using namespace vda5050pp::interface_agv::agv_description;
AGVDescription agv_description;

// Currently Mandatory fields (for message receiving)
agv_description.agv_id = "agv_id";
agv_description.manufacturer = "agv_manufacturer";
agv_description.serial_number = "serial_number";

// "volume" parameter float from 0-100
ParameterRange volume;
volume.key = "volume";
volume.ordinal_min = SerializedValue(0.0);
volume.ordinal_max = SerializedValue(100.0);

// "lhd" parameter can be either "left" or "right"
ParameterRange lhd;
lhd.key = "lhd";
lhd.value_set = {"left", "right"};

// "stationType" parameter can be either "typeA" or "typeB"
ParameterRange station_type;
station_type.key = "stationType";
station_type.value_set = {"typeA", "typeB"};

// "stationName" parameter can be either "s1", "s2" or "s3"
ParameterRange station_name;
station_name.key = "stationName";
station_name.value_set = {"s1", "s2", "s3"};

// "loadType" parameter can be either "EPAL" or "barrel"
ParameterRange load_type;
load_type.key = "loadType";
load_type.value_set = {"EPAL", "barrel"};

// "loadId" parameter can be any string
ParameterRange load_id;
load_id.key = "loadId";

// "height" parameter can be a float from 0.2 - 1.35
ParameterRange height;
height.key = "height";
height.ordinal_min = SerializedValue(0.2);
height.ordinal_max = SerializedValue(1.35);

// "depth" parameter can be a float from 0 - 0.65
ParameterRange depth;
depth.key = "depth";
depth.ordinal_min = SerializedValue(0.0);
depth.ordinal_max = SerializedValue(0.65);

// "side" parameter can be a either "conveyor" or "main"
ParameterRange side;
side.key = "side";
side.value_set = {"conveyor", "main"};

// The "pick" action declaration
ActionDeclaration pick_action;
pick_action.action_type = "pick";
pick_action.blocking_types = {vda5050pp::BlockingType::HARD}; // Can only be "HARD" blocking
pick_action.edge = false; // cannot be attached to edges
pick_action.node = true; // can be attached to nodes
pick_action.instant = true; // can be sent via instant actions
pick_action.parameter = {station_type, load_type}; // has two mandatory parameters
pick_action.optional_parameter = {lhd, station_name, load_id, height, depth, side}; // has six optional parameters

// The "drop" action declaration
ActionDeclaration drop_action;
drop_action.action_type = "drop";
drop_action.blocking_types = {vda5050pp::BlockingType::HARD}; // Can only be "HARD" blocking
drop_action.edge = false; // cannot be attached to edges
drop_action.node = true; // can be attached to nodes
drop_action.instant = true; // can be sent via instant actions
drop_action.parameter = {station_type, load_type}; // has two mandatory parameters
drop_action.optional_parameter = {lhd, station_name, load_id, height, depth, side}; // has six optional parameters

// The "honk" action
ActionDeclaration honk_action;
honk_action.action_type = "honk";
honk_action.blocking_types = {vda5050pp::BlockingType::NONE}; // Can only be "NONE" blocking
honk_action.edge = true; // Can be attached to edges
honk_action.node = true; // Can be attached to nodes
honk_action.instant = true; // Can be sent via instant actions
honk_action.parameter = {}; // has no mandatory parameters
honk_action.optional_parameter = {volume}; // has an optional volume parameter

// Put all action declarations into the agv_description
agv_description.supported_actions = {honk_action, pick_action, drop_action};
```

## 4. Running the Library

Finally everything is set up to start the library. The following snippet
shows the typical structure of a `main` function.

```c++
#include <memory>
#include <vda5050++/extra/console_logger.h>
#include <vda5050++/extra/mqtt_connector.h>
#include <vda5050++/interface_agv/handle.h>

std::unique_ptr<vda5050pp::interface_agv::Handle> library_handle_ptr;

int main() {
// Init AGV code...

auto agv_description = getAGVDescription(); // see section 3
auto logger_ptr = std::make_shared<vda5050pp::extra::ConsoleLogger>(); // Use a console logger

// Configure mqtt connector
vda5050pp::extra::MqttConnector::MqttOptions mqtt_opts;
mqtt_opts.username = "user";
mqtt_opts.password = "password";
mqtt_opts.server = "ssl://vda5050-broker.example.net";
auto connector = std::make_shared<vda5050pp::extra::MqttConnector>(agv_description, mqtt_opts); // Use the mqtt connector

//  the "magic" struct containing all agv handlers
vda5050pp::interface_agv::Handlers<MyNavigationHandler,
                                    MyActionHandler,
                                    MyPauseResumeHandler> handlers;

// Create library handle and start the library
library_handle_ptr = std::make_unique<vda5050pp::interface_agv::Handle>(
    agv_description, connector, handlers, logger_ptr);

// Either
// Spin the library with the desired number of threads
library_handle_ptr->spinParallel(4);

// Or
while (running) {
  agv.spinOnce(); // spin agv code
  library_handle_ptr->spinOnce(); // spin library code
}
}
```

It is recommended to store the Handle within a shared or unique ptr, because
the Handle is required to be in a fixed memory location.

There are different methods of spinning the library.
`spinParallel` is the most convenient method, because it takes
care of the thread creation. If you want to take care of the threads by yourself
`spinOnce``might be your choice, because it always returns after one Handler
call.

This snippet uses targets, which are part of the `extra` library. To include them, add the CMake target dependencies:
```CMake
target_link_libraries(${PROJECT_NAME} PUBLIC
libvda5050++::console_logger
libvda5050++::mqtt_connector
)
```