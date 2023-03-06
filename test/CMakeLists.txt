find_package(Threads REQUIRED)
find_package(Catch2 2.13.8 QUIET)

if(Catch2_FOUND)
  message(STATUS "Found Catch2 installation.")
else()
  CPMAddPackage(
    NAME Catch2
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION 2.13.8
  )
  list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
endif()

include(Catch)

add_compile_definitions(VDA5050PP_VERSION=${PROJECT_VERSION})
add_executable(vda5050++_test
  ${PROJECT_SOURCE_DIR}/test/main.cpp
  ${PROJECT_SOURCE_DIR}/test/src/console_logger.cpp
  ${PROJECT_SOURCE_DIR}/test/src/dump.cpp
  ${PROJECT_SOURCE_DIR}/test/src/order_factory.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_action_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_connector.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_continuous_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_odometry_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_pause_resume_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_step_based_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/blocking_queue.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/interruptable_timer.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/math/geometry.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/math/linear_path_length_calculator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/semaphore.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/action_manager.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/combined_tests.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/continuous_navigation.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/init_position.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/net_manager.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/parallel_launch_net.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/logic/sync_net.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/action_declared_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/header_target_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/header_version_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/order_append_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/order_graph_consistency_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/order_id_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/order_reachable_validator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/model/equality.cpp
)
target_link_libraries(vda5050++_test Catch2::Catch2 vda5050++ Threads::Threads)

target_include_directories(vda5050++_test PRIVATE ${PROJECT_SOURCE_DIR}/test/include)

# Let CTest discover the Catch2 test cases
catch_discover_tests(vda5050++_test)
