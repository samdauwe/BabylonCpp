# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET BabylonCpp)

# Print status message
message(STATUS "Lib ${TARGET}")


# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Meta information about the project
set(META_PROJECT_NAME        "BabylonCpp")
set(META_PROJECT_DESCRIPTION "BabylonCpp - A port of Babylon.js to C++")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)

# ============================================================================ #
#                       Project options                                        #
# ============================================================================ #

# Single Instruction Multiple Data (SIMD) support
set(OPTION_ENABLE_SIMD        false)

# Generate options-header
configure_file(options.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_options.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")


file(GLOB_RECURSE BABYLON_HEADERS
    ${INCLUDE_PATH}/*.h
    ${INCLUDE_PATH}/*.hpp
    )
file(GLOB_RECURSE BABYLON_SOURCES
    ${SOURCE_PATH}/*.cpp
    ${SOURCE_PATH}/*.c
    )

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
babylon_add_library(${TARGET}
    ${BABYLON_SOURCES}
    ${BABYLON_HEADERS}
)

if (OPTION_ENABLE_SIMD)
    target_compile_definitions(${TARGET} PRIVATE OPTION_ENABLE_SIMD)
endif()

# Export library for downstream projects
export(TARGETS ${TARGET} NAMESPACE ${META_PROJECT_NAME}:: FILE ${CMAKE_OUTPUT_PATH}/${TARGET}-export.cmake)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Libraries
target_include_directories(${TARGET}
    PRIVATE SYSTEM
    ${EARCUT_HPP_INCLUDE_DIRS}
    ${JSON_HPP_INCLUDE_DIRS}
    PUBLIC
    $<BUILD_INTERFACE:${STB_IMAGE_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${PROMISE_CPP_INCLUDE_DIR}>
)

target_compile_definitions(${TARGET} PUBLIC BABYLON_REPO_FOLDER="${CMAKE_SOURCE_DIR}")

# ============================================================================ #
#                        Include What You Use                                  #
# ============================================================================ #
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(BUILD_IWYU)
        find_program(IWYU_PATH NAMES include-what-you-use iwyu)
        if(NOT IWYU_PATH)
          message(FATAL_ERROR "Could not find the program include-what-you-use")
        endif()
        set_property(TARGET ${TARGET}
                     PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
    endif()
endif()


# ============================================================================ #
#                       Setup test environment                                 #
# ============================================================================ #

if(OPTION_BUILD_TESTS)
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()


# ============================================================================ #
#                       Deployment                                             #
# ============================================================================ #

# 'make install' to the correct location
if (NOT BABYLON_DISABLE_INSTALL)
    install(TARGETS ${TARGET}
        EXPORT   "${TARGET}-export"                    COMPONENT dev
        ARCHIVE  DESTINATION ${ARCHIVE_OUTPUT_PATH}    COMPONENT dev
        RUNTIME  DESTINATION ${EXECUTABLE_OUTPUT_PATH} COMPONENT runtime
        LIBRARY  DESTINATION ${LIBRARY_OUTPUT_PATH}    COMPONENT runtime
    )

    # Make the project importable from the install directory
    install(EXPORT "${TARGET}-export"
        DESTINATION ${TARGET}/${CMAKE_OUTPUT_PATH}
    )
endif()
