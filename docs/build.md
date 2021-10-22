# Table of Contents
- [Table of Contents](#table-of-contents)
- [Build options](#build-options)
- [Standalone (CMake package)](#standalone-cmake-package)
  - [Get the libVDA5050++ source code](#get-the-libvda5050-source-code)
  - [Configuring the build](#configuring-the-build)
  - [Building/Installing](#buildinginstalling)
    - [Build:](#build)
    - [Test:](#test)
    - [Install](#install)
    - [Build the documentation](#build-the-documentation)
  - [Include the library from within your CMake project](#include-the-library-from-within-your-cmake-project)
- [Integration into an existing project (Git Submodule)](#integration-into-an-existing-project-git-submodule)
  - [Setup the Git Submodule](#setup-the-git-submodule)
  - [Configuring CMakeLists.txt](#configuring-cmakeliststxt)
    - [Select components](#select-components)
    - [Include the subdirectory](#include-the-subdirectory)
    - [Link targets](#link-targets)
  - [Direct build (not recommended)](#direct-build-not-recommended)
- [Documentation](#documentation)
  - [Without Code Documentation](#without-code-documentation)
  - [With Code Documentation](#with-code-documentation)

# Build options

You can choose to install the library as a [CMake Package](#standalone-cmake-package)
or embed it into your project as a [git submodule](#integration-into-an-existing-project-git-submodule).

# Standalone (CMake package)
## Get the libVDA5050++ source code

Clone the [libVDA5050++ Project](https://git.openlogisticsfoundation.org/silicon-economy/libraries/vda5050/libvda5050pp)

```
git clone git@git.openlogisticsfoundation.org:silicon-economy/libraries/vda5050/libvda5050pp.git
```

Checkout dev branch in "libvda5050pp" folder if you want to continue with development, otherwise skip this step and continue with master branch

```
git checkout dev 
```

## Configuring the build

The libVDA505++ comes with some [extra components](/extra)
(see `extra/` directory). These components can be enabled/disabled at will.

The default configuration is achieved by running
the following command inside of the libVDA5050++ project root:

```sh
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release \ 
      -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10 \
      -DBUILD_DOCS=ON \ # enable documentation
      -DBUILD_TESTING=ON  # enable testing
```

If paho was installed in a non-default location, you need to
specify it's install path with `-DCMAKE_PREFIX_PATH=<path>`.
Optionally setup a custom install path with `-DCMAKE_INSTALL_PREFIX=<path>`.

Available extra components can be enabled via:

- MQTTConnector (needs JSON_MODEL): `-DUSE_EXTRA_MQTT_CONNECTOR=ON`
- JSON_MODEL: `-DUSE_EXTRA_JSON_MODEL=ON`
- LoggerUtils: `-DUSE_EXTRA_LOGGER_UTILS=ON`

## Building/Installing

### Build:

```sh
cmake --build build --target all
```

### Test:

Run the following command from inside of the `build/` directory:

```sh
ctest
```

### Install

(may need to be run as privileged user)

```sh
cmake --build build --target install
```

### Build the documentation

```sh
cmake --build build --target mkdocs
```

view it by running `mkdocs serve` inside of the `build/` directory.

## Include the library from within your CMake project

Once installed, you can locate the library via
CMake's `find_package` command:

```cmake
find_package(libvda5050++ "1.0" REQUIRED COMPONENTS
  # uncomment the desired components

  # mqtt_connector
  # json_model
  # logger_utils
)
```

All exported targets are prefixed with the ```libvda5050++::```
namespace:

- `libvda5050++::vda5050++` the main library
- `libvda5050++::mqtt_connector` extra component
- `libvda5050++::json_model` extra component
- `libvda5050++::logger_utils` extra component

Your CMakeLists.txt may contain the following lines:

```cmake
find_package(libvda5050++ "1.0" REQUIRED COMPONENTS
  mqtt_connector
  json_model
)
add_executable(MY_APP main.cpp)
target_link_libraries(MY_APP PUBLIC
  libvda5050++::vda5050++
  libvda5050++::mqtt_connector
)
```

# Integration into an existing project (Git Submodule)

## Setup the Git Submodule

In this tutorial we assume you have a `third party` folder for third party software you use in your project. If you have another file structure replace the `third_party` folder in the commands with your file structure. 

To add the libVDA5050++ as a submodule to your project execute the following command. Notice that your project must already be a git project!

```sh
git submodule add git@git.openlogisticsfoundation.org:silicon-economy/libraries/vda5050/libvda5050pp.git third_party
```

To download the added submodule to your project execute the following command:

```sh
git submodule update --init --recursive
```

For more information about `git submodules` visit the official [Git Submodule Documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

To Process how to integrate the submodule to your `Cmake Project` is described in the [Quick Start Guide](quickstart.md#1-integrating-the-library) 

## Configuring CMakeLists.txt

### Select components

The libVDA505++ comes with some [extra components](/extra)
(see `extra/` directory). These components can be enabled/disabled at will.
To enable some extra targets, you need to add a selection of the following
lines to your top-level CMakeLists.txt before the `add_subdirectory`
command in the next step.

```cmake
set(USE_EXTRA_MQTT_CONNECTOR ON) # enable mqtt_connector target
set(USE_EXTRA_JSON_MODEL ON) # enable json_model target
set(USE_EXTRA_LOGGER_UTILS ON) # enable logger_utils target
```

### Include the subdirectory

Add the following cmake commands to your top-level CMakeLists.txt:

With CMake, you can directly include the libvda5050++ directory like this:

```cmake
## Add libvda5050++ subdirectory
add_subdirectory(third_party/libvda5050pp)
```

### Link targets

To link the library with your AGV project, you can depend on the `vda5050++` target:

```cmake
target_link_libraries(${PROJECT_NAME} PUBLIC
  vda5050++
)
```

_NOTE:_ When including the library as a CMake subdirectory, all targets are **not** prefixed with `libvda5050++::`.

## Direct build (not recommended)

If you can not include the project via CMake, you can
still build it directly. 

To build libVDA5050++ together with your project, you need to add the `libvda5050pp/include` and `libvda5050pp/third_party/SimplePTN/include` folder to your include path. All sources in `libvda5050pp/src` have to be built. For the main sources, the only library used is the STL (c++17). 
_Note: the project requires c++17 language features._

# Documentation

For viewing the documentation inside your browser, it's necessary to install `mkdocs`.
## Without Code Documentation

Inside of the project root folder run:

```sh
mkdocs serve
```

You can now [view](http://127.0.0.1:8000/) the documentation in your browser.

_To recompile the diagrams, run `docs/diagrams/generate.sh`_

## With Code Documentation


Install `mkdocs` and `doxygen`, then run the mkdocs build target inside of the `build` folder:


```shell
cmake --build build --target mkdocs
```

_Make sure the build was configured with `BUILD_DOCS=ON`.
See [Configuring the build](#configuring-the-build)._

`PROJECT_ROOT/build/site` now contains the documentation. After building, the documentation can be served by running the following command inside the `build` folder:

```sh
mkdocs serve
```

You can now [view](http://127.0.0.1:8000/) the documentation in your browser.