cmake_minimum_required(VERSION 3.16)# Adds instrumentation to all targets

# Do not check system dependencies for changes, when rebuilding. 
# Ninja build scripts always flag them as dirty, so each time the project is built
# every file is (wrongly) forcefully rebuilt.
if (CMAKE_GENERATOR MATCHES "Ninja")
    file(
        WRITE "${CMAKE_BINARY_DIR}/GNUMakeRulesOverwrite.cmake" 
            "STRING(REPLACE \"-MD\" \"-MMD\" CMAKE_DEPFILE_FLAGS_C \"\${CMAKE_DEPFILE_FLAGS_C}\")\n"
            "STRING(REPLACE \"-MD\" \"-MMD\" CMAKE_DEPFILE_FLAGS_CXX \"\${CMAKE_DEPFILE_FLAGS_CXX}\")\n"
    )
    set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_BINARY_DIR}/GNUMakeRulesOverwrite.cmake" CACHE INTERNAL "")
endif()


project(libvda5050++
        VERSION 1.0.0
        DESCRIPTION "Helper library for building VDA 5050 compliant software"
        LANGUAGES CXX)


# declare CPM
include(cmake/CPM.cmake)
option(LIBVDA5050PP_SPTN_DEP_LOCAL "libVDA5050++: Use SimplePTN soruces next to libvda5050pp folder" OFF)
if(LIBVDA5050PP_SPTN_DEP_LOCAL)
  set(CPM_SimplePTN_SOURCE ${PROJECT_SOURCE_DIR}/../simpleptn)
endif()


# Compiler flag function
include(CheckCXXCompilerFlag)
function(set_compiler_flag flag)
    check_cxx_compiler_flag("${flag}" flag_supported)
    if(flag_supported)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    else()
        message(STATUS "CXX_FLAG ${flag} not supported")
    endif()
    unset(flag_supported CACHE)
endfunction()

set_compiler_flag("-Wall")
set_compiler_flag("-Wextra")
set_compiler_flag("-pedantic")

option(CI_ENABLE_ALL "Enable all build options (used by CI)" OFF)
if (CI_ENABLE_ALL)
    set(BUILD_DOCS ON)
    set(BUILD_TESTING ON)
    set(CODE_COVERAGE ON)
    set(USE_EXTRA_LOGGER_UTILS  ON)
    set(USE_EXTRA_JSON_MODEL ON)
    set(USE_EXTRA_MQTT_CONNECTOR ON)
endif()

# If this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # use c++ instead of g++
    set(CMAKE_CXX_EXTENSIONS OFF)

    # include doxygen
    option(BUILD_DOCS "Build documentation with doxygen and mkdocs" OFF)
    if (BUILD_DOCS)
        add_subdirectory(docs)
    endif()
endif()

# include tests
include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()

# only use code coverage with clang (include here, such that child projects do not initialize
#                                    code-cov before us)
if (BUILD_TESTING AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Include to enable code coverage for SonarQube
    include(cmake/code-coverage.cmake)
endif()

# include SimplePTN
if (NOT LIBVDA5050PP_SPTN_DEP_LOCAL)
    find_package(SimplePTN 1.0.0 QUIET)
endif()

if (SimplePTN_FOUND)
        message(STATUS "Found SimplePTN installation.")
else()
    set(SimplePTN_URL "https://git.openlogisticsfoundation.org/silicon-economy/libraries/vda5050/simpleptn.git"
        CACHE STRING "Overwrite the SimplePTN repo URL")
    cpmaddpackage( NAME SimplePTN
        GIT_REPOSITORY ${SimplePTN_URL}
        GIT_TAG main
        EXCLUDE_FROM_ALL NO
        )
endif()


# include libvda5050 source code
add_subdirectory(src)

# include extra dirs
add_subdirectory(extra)


# only use code coverage with clang
if (CODE_COVERAGE AND BUILD_TESTING AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_code_coverage(vda5050++_test) 
endif()

# Install part #################################################################

# generate the config file that is includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libvda5050++Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version matching file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++ConfigVersion.cmake"
  VERSION "${libvda5050++_VERSION_MAJOR}.${libvda5050++_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install helper files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libvda5050++Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libvda5050++ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)