# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Samples)

# Print status message
message(STATUS "Lib ${TARGET}")

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Meta information about the project
set(META_PROJECT_NAME        "Samples")
set(META_PROJECT_DESCRIPTION "")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}/samples")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src/samples")

file(GLOB_RECURSE BABYLON_SAMPLES_HEADERS
    ${INCLUDE_PATH}/*.h
    ${INCLUDE_PATH}/*.hpp
    )
file(GLOB_RECURSE BABYLON_SAMPLES_SOURCES
    ${SOURCE_PATH}/*.cpp
    ${SOURCE_PATH}/*.c
    )

# Group source files
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${BABYLON_SAMPLES_HEADERS} ${BABYLON_SAMPLES_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
babylon_add_library(${TARGET}
    ${BABYLON_SAMPLES_SOURCES}
    ${BABYLON_SAMPLES_HEADERS}
)

target_include_directories(${TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    imgui_utils
    BabylonCpp
    Extensions
    MaterialsLibrary
    ProceduralTexturesLibrary
    Loaders
    json_hpp
)

if (WIN32)
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/ignore:4217 /ignore:4286 /ignore:4049")
endif()


# ============================================================================ #
#                       Setup test environment                                 #
# ============================================================================ #
# No Test here

# ============================================================================ #
#                       make samples info                                      #
# ============================================================================ #
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make_samples_info/make_samples_info.py
    COMMAND_ECHO STDOUT
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin
)

# ============================================================================ #
#                       Deployment                                             #
# ============================================================================ #

# 'make install' to the correct location
if (NOT BABYLON_DISABLE_INSTALL)
    install(TARGETS ${TARGET}
        EXPORT   ${TARGET}Config
        ARCHIVE  DESTINATION ${ARCHIVE_OUTPUT_PATH}
        RUNTIME  DESTINATION ${EXECUTABLE_OUTPUT_PATH}
        LIBRARY  DESTINATION ${LIBRARY_OUTPUT_PATH}
    )
    install(DIRECTORY include/ DESTINATION ${INCLUDE_OUTPUT_PATH})

    # Make the project importable from the install directory
    install(EXPORT ${TARGET}Config
        DESTINATION ${TARGET}/${CMAKE_OUTPUT_PATH}
    )

    # Make the project importable from the build directory
    export(TARGETS ${TARGET}
        FILE ${TARGET}Config.cmake
    )
endif()

# Copy asset directories
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/glTF-Sample-Models")
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/scenes")
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/textures")
if (EXISTS "${CMAKE_SOURCE_DIR}/assets/samples_info.json")
    copy_resource_files("${CMAKE_SOURCE_DIR}/assets/samples_info.json")
endif()
