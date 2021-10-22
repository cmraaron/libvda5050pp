# This file contains the extra library definition

option(USE_EXTRA_LOGGER_UTILS "Enable logger_utils component" ON)
option(USE_EXTRA_JSON_MODEL "Enable json_model component" OFF)
option(USE_EXTRA_MQTT_CONNECTOR "Enable mqtt_connector component" OFF)

if (USE_EXTRA_MQTT_CONNECTOR OR USE_EXTRA_CLI_CONNECTOR)
  set(USE_EXTRA_JSON_MODEL ON CACHE BOOL "Enable json_model component" FORCE)
endif()

if(USE_EXTRA_LOGGER_UTILS)
  message(STATUS "Building extra with logger_utils")
  add_subdirectory(logger_utils)
endif()

if(USE_EXTRA_JSON_MODEL)
  message(STATUS "Building extra with json_model")
  add_subdirectory(json_model)
endif()

if(USE_EXTRA_MQTT_CONNECTOR)
  message(STATUS "Building extra with mqtt_connector")
  add_subdirectory(mqtt_connector)
endif()