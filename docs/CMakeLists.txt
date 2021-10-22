# Copy utlilties ###################################################################################

# Mkdocs configuration can be copied during configuration
file(COPY
  ${PROJECT_SOURCE_DIR}/mkdocs.yml
  DESTINATION ${PROJECT_BINARY_DIR}
)

# Target for copying files to build directory
add_custom_target(copy_mkdocs_files)

function(mkdocs_copy_file src dest)
  add_custom_command(TARGET copy_mkdocs_files
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${src} ${dest})
endfunction()

function(mkdocs_copy_dir src dest)
  add_custom_command(TARGET copy_mkdocs_files
                     COMMAND ${CMAKE_COMMAND} -E
                         copy_directory ${src} ${dest})
endfunction()

set(mkdocs_raw_directory ${PROJECT_BINARY_DIR}/docs)
mkdocs_copy_dir(${PROJECT_SOURCE_DIR}/docs/resources ${mkdocs_raw_directory}/resources)
mkdocs_copy_file(${PROJECT_SOURCE_DIR}/docs/build.md ${mkdocs_raw_directory})
mkdocs_copy_file(${PROJECT_SOURCE_DIR}/docs/extra.md ${mkdocs_raw_directory})
mkdocs_copy_file(${PROJECT_SOURCE_DIR}/docs/index.md ${mkdocs_raw_directory})
mkdocs_copy_file(${PROJECT_SOURCE_DIR}/docs/install.md ${mkdocs_raw_directory})
mkdocs_copy_file(${PROJECT_SOURCE_DIR}/docs/quickstart.md ${mkdocs_raw_directory})
####################################################################################################

# Doxygen build ####################################################################################
find_package(Doxygen REQUIRED)
set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs/doxygen)
set(DOXYGEN_EXTRACT_ALL YES)

doxygen_add_docs(
  doxygen
  ${PROJECT_SOURCE_DIR}/extra
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/third_party/common/include
)
####################################################################################################

# Mkdocs build #####################################################################################
find_program(MKDOCS mkdocs)

add_custom_target(mkdocs)
add_custom_command(TARGET mkdocs
                   COMMAND ${MKDOCS} build
                   WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
add_dependencies(mkdocs copy_mkdocs_files)
add_dependencies(mkdocs doxygen)
####################################################################################################