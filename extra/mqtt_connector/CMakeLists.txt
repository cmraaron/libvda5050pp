find_package(PahoMqttCpp REQUIRED)

add_library(mqtt_connector STATIC
  src/mqtt_connector.cpp
)
target_link_libraries(mqtt_connector PUBLIC vda5050++)
target_link_libraries(mqtt_connector PRIVATE json_model PahoMqttCpp::paho-mqttpp3-static)
target_include_directories(mqtt_connector PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS mqtt_connector
  EXPORT mqtt_connectorTargets
  DESTINATION lib/${PROJECT_NAME}
  COMPONENT mqtt_connector
)

install(
  DIRECTORY include/vda5050++
  DESTINATION include
  COMPONENT mqtt_connector
)

# make targets available
install(EXPORT mqtt_connectorTargets
  FILE mqtt_connectorTargets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# generate the config file that is includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/mqtt_connector-config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install helper files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/mqtt_connector-config.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)