# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET BabylonCpp)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "Lib ${TARGET}")


# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)

# Meta information about the project
set(META_PROJECT_NAME        "BabylonCpp")
set(META_PROJECT_DESCRIPTION "BabylonCpp - A port of Babylon.js to C++")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "4")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")

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

source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${BABYLON_HEADERS} ${BABYLON_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_SOURCES}
    ${BABYLON_HEADERS}
)
babylon_target_clang_tidy(${TARGET})


if (OPTION_ENABLE_SIMD)
    target_compile_definitions(${TARGET} PRIVATE OPTION_ENABLE_SIMD)
endif()


# Create namespaced alias
add_library(${META_PROJECT_NAME}::${TARGET} ALIAS ${TARGET})

# Export library for downstream projects
export(TARGETS ${TARGET} NAMESPACE ${META_PROJECT_NAME}:: FILE ${CMAKE_OUTPUT_PATH}/${TARGET}-export.cmake)

# Create feature detection header
# Compilers: https://cmake.org/cmake/help/v3.1/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
# Feature: https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html

# Check for availability of module; use pre-generated version if not found
if (WriterCompilerDetectionHeaderFound)
    write_compiler_detection_header(
        FILE ${FEATURE_FILE}
        PREFIX ${BABYLON_UPPER}
        COMPILERS AppleClang Clang GNU MSVC
        FEATURES cxx_alignas cxx_alignof cxx_constexpr cxx_final cxx_noexcept cxx_nullptr cxx_sizeof_member cxx_thread_local
        VERSION 3.2
    )
else()
    file(
        COPY ${PROJECT_SOURCE_DIR}/codegeneration/${TARGET}_features.h
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/${TARGET}
        USE_SOURCE_PERMISSIONS
    )
endif()

# Create API export header
generate_export_header(${TARGET}
    EXPORT_FILE_NAME  ${EXPORT_FILE}
    EXPORT_MACRO_NAME ${EXPORT_MACRO}
)

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

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${BABYLON_UPPER}_STATIC_DEFINE>
    BABYLON_REPO_FOLDER="${CMAKE_SOURCE_DIR}"
)
# Compile options
target_compile_options(${TARGET}
    PUBLIC
)

# Set library ouput name
set_target_properties(${TARGET}
    PROPERTIES  PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}"
                OUTPUT_NAME $<LOWER_CASE:${TARGET}>
                VERSION ${META_VERSION}
                SOVERSION ${META_VERSION}
)

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
