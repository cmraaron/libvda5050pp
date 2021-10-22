# Table of Contents

- [Table of Contents](#table-of-contents)
- [libVDA5050++](#libvda5050)
  - [What the libVDA5050++ takes care of](#what-the-libvda5050-takes-care-of)
- [Installation](#installation)
- [Building the library](#building-the-library)
- [Getting Started](#getting-started)
- [Developer Documentation](#developer-documentation)

***NOTE: The libVDA5050++ library and the VDA5050 standard are still under development. Therefore, changes to the library interface in future releases cannot be ruled out.***

# libVDA5050++

libVDA5050++ is a complete and open source implementation of the standard VDA5050 version 1.1.0 ([VDA5050 Github Repo](https://github.com/VDA5050/VDA5050) / [VDA5050 standard v1.1.0](https://github.com/VDA5050/VDA5050/tree/1.1.0)). The aim of the VDA5050 standard is to standardize the interface for communication of automated guided vehicles (AGVs) between the control system and the AGVs. Today's systems are mainly isolated solutions, which means that the control of AGVs is proprietary and therefore manufacturer-specific. This means that AGVs from different manufacturers cannot be controlled by a control system of a specific manufacturer. The VDA5050 standard, which was developed by the German Association of the Automotive Industry (Verband der Automobilindustrie e. V. (VDA)) VDA and Mechanical Engineering Industry Association (Verband Deutscher Maschinen- und Anlagenbau (VDMA)), is intended to overcome this situation. 

![Structure of the libVDA5050++ implementation](docs/resources/libVDA5050++_generic_small.png)

The libVDA5050++ offers a open source implementation of the standard, which enables manufacturers of AGvVs to quickly align their vehicles with the standard without major effort. The libVDA5050++ offers a generic interface to the control system (Master Control Adapter), which is implemented in a middleware-neutral manner which can be quickly adapted to new requirements like different middleware solutions (OPC UA, MQTT, ROS, etc.). Furthermore, the library has a lean interface to the vehicle site (AGV Adapter), which can be used to forward upcoming driving maneuvers received via the VDA5050 standard to the vehicle and forward received status information VDA5050-compliant from the vehicle back to the control system. The entire VDA5050 logic is encapsulated within the libVDA5050++.

## What the libVDA5050++ takes care of

List of the most important functionalities of the libVDA5050++:

- Validate Orders (e.g. check if actions are available) and automatically respond with status/error messages if possible
- Keep track of timings (periodic status messages)
- Supervise AGV navigation (goals, pausing, resuming, stopping)
- Notify AGV about pending actions of the order based on it's position
- Notify AGV / Master Control about certain state changes
- Provide an easy access to the current state
- Process the AGV's position

# Installation

You can choose to install the library as a CMake package or simply
include the library as a submodule.
Here you can find a guide to install / check all prerequisites to setup the libVDA5050++ on your system: [Installation Guide](docs/install.md) 

# Building the library

You have successfully setup your system. Now you can start with building the library on your, following:  [Building libVDA5050++](docs/build.md)
# Getting Started

You want to write your own application based on the libVDA5050++? Here is your guide: [Quick Start Guide](docs/quickstart.md) 

# Developer Documentation 

Coming soon.
