# Table of Contents
- [Table of Contents](#table-of-contents)
- [Prerequisites for building the code](#prerequisites-for-building-the-code)
- [Setting up the system](#setting-up-the-system)
  - [Install cmake](#install-cmake)
  - [Install Clang v9 / v10](#install-clang-v9--v10)
    - [Ubuntu 20.04](#ubuntu-2004)
    - [Ubuntu 18.04](#ubuntu-1804)
  - [Install Catch2 (optional, will be downloaded via CPM, if not installed)](#install-catch2-optional-will-be-downloaded-via-cpm-if-not-installed)
  - [Install git](#install-git)
  - [Optional Software](#optional-software)
    - [Install Mkdocs](#install-mkdocs)
    - [Install Doxygen](#install-doxygen)
    - [Install nlohmann::json (optional, will be downloaded via CPM, if not installed)](#install-nlohmannjson-optional-will-be-downloaded-via-cpm-if-not-installed)
    - [Install paho.mqtt.cpp](#install-pahomqttcpp)

# Prerequisites for building the code

The installation guide is currently limited to a Debian-based system. In this tutorial we go through the installation based on an Ubuntu 18.04 or 20.04 is installed. 

Software prerequisites:
- cmake
- clang
- catch2
- git

Optional software:
- mkdocs
- doxygen
- [nlohmann::json](https://github.com/nlohmann/json) (used by: ```extra::json_model```, ```extra::mqtt_connector```)
- [paho.mqtt.cpp](https://github.com/eclipse/paho.mqtt.cpp) (used by: ```extra::mqtt_connector```)

# Setting up the system

In the following steps, we will install and setup all necessary software.

## Install cmake

You can install `cmake` using the following command:

```sh
sudo apt install cmake
```

## Install Clang v9 / v10

By default lbVDA5050++ is build with clang. In the a first step we will setup clang on your Ubuntu 18.04 or 20.04 system.

### Ubuntu 20.04 

For Ubuntu 20.04 clang v10 will be installed. You can install it with the following command:

```shell
sudo apt install clang
```

### Ubuntu 18.04

For installing clang 9 on Ubuntu 18.04 you have to add a repository. Perform the following commands to install clang v9:

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt install clang-9
```

## Install Catch2 (optional, will be downloaded via CPM, if not installed)
We recommend to install `catch2` beside your libVDA5050++. For further information visit [Catch2 Github](https://github.com/catchorg/Catch2).

Recommended file structure:

```text
Development Folder
 |- catch2
 |- Your Project / libvda5050pp
```

Clone the `catch2` test-framework. 

```sh
git clone https://github.com/catchorg/Catch2.git
```

Navigate inside the `catch2` folder

```sh
cd catch2
```

Change the branch to `catch2` version 2 (v2.x)

```sh
git checkout v2.x
```

Configure the build process

```sh
cmake -Bbuild -H. -DBUILD_TESTING=OFF
```

Build and install `catch2` on your system

```sh
sudo cmake --build build/ --target install
```

## Install git

For Debian-based distribution, such as Ubuntu, use the following command. For further information visit the [Git Homepage](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git).

```sh
sudo apt install git-all
```

## Optional Software

`Mkdocs` and `Doxygen` are optional software. You can install them to build the documentation on your system. The complete documentation is also available on the Gitlab-Pages in the libVDA5050++ repository.

### Install Mkdocs

To install `Mkdocs` on your system, execute the following command:

```sh
pip3 install mkdocs
```

If you don't have `python3` and `pip3` installed on your system you can also install mkdocs via `apt`. We encountered the problem that sometimes code blocks in the documentation are displayed as a single line with the `apt` version of `mkdocs`. Hence we recommend to install `mkdocs` with `pip3`.

```sh
sudo apt install mkdocs
```
### Install Doxygen

To install `Doxygen` on your system, execute the following command:

```sh
sudo apt install doxygen
```

### Install nlohmann::json (optional, will be downloaded via CPM, if not installed)

Clone the project repository:

```sh
git clone --depth 1 --branch v3.9.1 https://github.com/nlohmann/json
```

Configure the build directory:

```sh
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10
```
Optionally add the `-DCMAKE_INSTALL_PREFIX=$HOME/.local` Flag to install
the library locally.

Finally install the library:

```sh
cmake --build build --target install
```

### Install paho.mqtt.cpp

Prerequisites:

```sh
sudo apt-get install libssl-dev
```

Build and install paho.mqtt.c:

```
git clone --depth 1 --branch v1.3.8 https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10
sudo cmake --build build/ --target install
sudo ldconfig
```

Optionally add the `-DCMAKE_INSTALL_PREFIX=$HOME/.local` flag to install
the library locally.


Build and install paho.mqtt.cpp:

```
git clone --depth 1 --branch v1.2.0 https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
  -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10
sudo cmake --build build/ --target install
sudo ldconfig
```

If the `paho.mqtt.c` library was installed locally, add the `-DCMAKE_PREFIX_PATH=$HOME/.local` flag,
to locate it.

Optionally add the `-DCMAKE_INSTALL_PREFIX=$HOME/.local` flag to install
the (paho.mqtt.cpp) library locally.