find_package(nlohmann_json 3.9.1 QUIET)
if (nlohmann_json_FOUND)
  message(STATUS "Found nlohman_json installation.")
else()
  CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.9.1
    GITHUB_REPOSITORY nlohmann/json
    OPTIONS 
      "JSON_BuildTests OFF"
  )
endif()


add_library(json_model STATIC
  src/json_model.cpp
)
target_link_libraries(json_model PUBLIC vda5050++ nlohmann_json::nlohmann_json)
target_include_directories(json_model PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS json_model
  EXPORT json_modelTargets
  DESTINATION lib/${PROJECT_NAME}
  COMPONENT json_model
)

install(
  DIRECTORY include/vda5050++
  DESTINATION include
  COMPONENT json_model
)

# make targets available
install(EXPORT json_modelTargets
  FILE json_modelTargets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# generate the config file that is includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/json_model-config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install helper files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/json_model-config.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)