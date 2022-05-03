find_package(Threads REQUIRED)

# The main libvda5050++.so
add_library(vda5050++ SHARED
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/common/exception.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/interface_agv/const_handle_accessor.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/interface_agv/handle_accessor.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/action_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/cancel_net.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/continuous_navigation_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/continuous_navigation_single_step_task.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/dangling_net.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/drive_to_node_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/instant_actions_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/logic.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/net_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/parallel_launch_net.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/pause_resume_action_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/sync_net.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logic/task_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/messages/message_processor.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/messages/messages.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/messages/state_update_timer.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/state_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/action_declared_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/header_target_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/header_version_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/order_action_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/order_append_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/order_graph_consistency_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/order_id_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/order_reachable_validator.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/validation_provider.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/action_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/continuous_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/handle.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/logger.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/odometry_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/pause_resume_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/battery.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/loads.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/odometry.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/operating_mode.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/results.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/step_based_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/safety_state.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/interface_agv/status/visualization.cpp
)

set_target_properties(vda5050++ PROPERTIES
                      VERSION ${PROJECT_VERSION}
                      SOVERSION 0
)

target_link_libraries(vda5050++ PUBLIC SimplePTN Threads::Threads)
target_include_directories(vda5050++ PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(vda5050++ PUBLIC cxx_std_17)

## Check platform deps #########################################################
include(CheckCXXSymbolExists)
check_cxx_symbol_exists(localtime_r "ctime" HAVE_CTIME_LOCALTIME_R)
if(${HAVE_CTIME_LOCALTIME_R})
  target_compile_definitions(vda5050++ PRIVATE HAVE_CTIME_LOCALTIME_R)
endif()
################################################################################


## Install part ################################################################
# gather install targets
install(TARGETS vda5050++ EXPORT libvda5050++Targets)
install(DIRECTORY ../include/vda5050++ DESTINATION include)
# make targets available
install(EXPORT libvda5050++Targets
  FILE libvda5050++BaseTargets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# make targets available, if this is a subdirectory
export(EXPORT libvda5050++Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++BaseTargets.cmake"
)
################################################################################

# This leads to problems with ROS
# target_compile_options(vda5050++ PUBLIC "$<$<CONFIG:DEBUG>:-D_GLIBCXX_DEBUG>")