# object library
add_library(logger_utils STATIC
  src/console_logger.cpp
  src/file_logger.cpp
  src/multiplex_logger.cpp
  src/pretty_logger.cpp
  src/stream_logger.cpp
)
target_link_libraries(logger_utils PUBLIC vda5050++)
target_include_directories(logger_utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


install(TARGETS logger_utils
  EXPORT logger_utils-config
  DESTINATION lib/${PROJECT_NAME}
  COMPONENT logger_utils
)
install(
  DIRECTORY include/vda5050++
  DESTINATION include
  COMPONENT logger_utils
)

install(
  EXPORT logger_utils-config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)