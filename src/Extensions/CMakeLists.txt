# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Extensions)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "Lib ${TARGET}")

# Namespace
set(BABYLON_NAMESPACE babylon)

# Set API export file and macro
string(TOUPPER ${BABYLON_NAMESPACE} TARGET_UPPER)
set(FEATURE_FILE "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_features.h")
set(EXPORT_FILE  "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_api.h")
set(EXPORT_MACRO "${TARGET_UPPER}_SHARED_EXPORT")

# ============================================================================ #
#                       Project configuration options                          #
# ============================================================================ #
option(BUILD_SHARED_LIBS     "Build shared instead of static libraries."     ON)
option(OPTION_BUILD_TESTS    "Build tests."                                  ON)

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)

# Meta information about the project
set(META_PROJECT_NAME        "Extensions")
set(META_PROJECT_DESCRIPTION "Extensions for BabylonCpp")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "3")
set(META_VERSION_MINOR       "3")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source and Tests path
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TESTS_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Header files
file(GLOB EXTENSIONS_HDR_FILES  ${INCLUDE_PATH}/extensions/dynamicterrain/*.h
                                ${INCLUDE_PATH}/extensions/entitycomponentsystem/*.h
                                ${INCLUDE_PATH}/extensions/entitycomponentsystem/detail/*.h
                                ${INCLUDE_PATH}/extensions/entitycomponentsystem/util/*.h
                                ${INCLUDE_PATH}/extensions/hexplanetgeneration/*.h
                                ${INCLUDE_PATH}/extensions/hexplanetgeneration/terrain/*.h
                                ${INCLUDE_PATH}/extensions/hexplanetgeneration/utils/*.h
                                ${INCLUDE_PATH}/extensions/navigation/*.h
                                ${INCLUDE_PATH}/extensions/navigation/rvo2/*.h
                                ${INCLUDE_PATH}/extensions/navigationmesh/*.h
                                ${INCLUDE_PATH}/extensions/noisegeneration/*.h
                                ${INCLUDE_PATH}/extensions/pathfinding/*.h
                                ${INCLUDE_PATH}/extensions/polyhedron/*.h
                                ${INCLUDE_PATH}/extensions/treegenerators/*.h)

set(BABYLON_EXTENSIONS_HEADERS
    ${EXTENSIONS_HDR_FILES}
)

# Source files
file(GLOB EXTENSIONS_SRC_FILES  ${SOURCE_PATH}/extensions/dynamicterrain/*.cpp
                                ${SOURCE_PATH}/extensions/entitycomponentsystem/*.cpp
                                ${SOURCE_PATH}/extensions/entitycomponentsystem/detail/*.cpp
                                ${SOURCE_PATH}/extensions/hexplanetgeneration/*.cpp
                                ${SOURCE_PATH}/extensions/hexplanetgeneration/terrain/*.cpp
                                ${SOURCE_PATH}/extensions/hexplanetgeneration/utils/*.cpp
                                ${SOURCE_PATH}/extensions/navigation/*.cpp
                                ${SOURCE_PATH}/extensions/navigation/rvo2/*.cpp
                                ${SOURCE_PATH}/extensions/navigationmesh/*.cpp
                                ${SOURCE_PATH}/extensions/noisegeneration/*.cpp
                                ${SOURCE_PATH}/extensions/polyhedron/*.cpp
                                ${SOURCE_PATH}/extensions/treegenerators/*.cpp)

set(BABYLON_EXTENSIONS_SOURCES
    ${EXTENSIONS_SRC_FILES}
)

# Group source files
set(HEADER_GROUP "Header Files (API)")
set(SOURCE_GROUP "Source Files")
source_group_by_path(${INCLUDE_PATH} "\\\\.h$|\\\\.hpp$"
    ${HEADER_GROUP} ${BABYLON_EXTENSIONS_HEADERS})
source_group_by_path(${SOURCE_PATH}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${SOURCE_GROUP} ${BABYLON_EXTENSIONS_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_EXTENSIONS_SOURCES}
    ${BABYLON_EXTENSIONS_HEADERS}
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${TARGET} ALIAS ${TARGET})

# Create feature detection header
# Compilers: https://cmake.org/cmake/help/v3.1/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
# Feature: https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html

# Check for availability of module; use pre-generated version if not found
if (WriterCompilerDetectionHeaderFound)
    write_compiler_detection_header(
        FILE ${FEATURE_FILE}
        PREFIX ${TARGET_UPPER}
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

# Project options
set_target_properties(${TARGET}
    PROPERTIES ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${JSON_HPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    PUBLIC
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    PRIVATE
    BabylonCpp
)

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${TARGET_UPPER}_STATIC_DEFINE>
)

# Compile options
target_compile_options(${TARGET}
    PRIVATE
    PUBLIC
)

# Set library ouput name
set_target_properties(${TARGET}
    PROPERTIES  PREFIX "lib"
                OUTPUT_NAME $<LOWER_CASE:${TARGET}>
                VERSION ${META_VERSION}
                SOVERSION ${META_VERSION}
)

# ============================================================================ #
#                       Setup test environment                                 #
# ============================================================================ #

# Check if tests are enabled
if(OPTION_BUILD_TESTS AND EXISTS ${TESTS_PATH})

# Function: Build test and add command to execute it via target 'test'
function(add_test_without_ctest target)
    add_subdirectory(${target})

    if(NOT TARGET ${target})
        return()
    endif()

    add_dependencies(test ${target})
    add_custom_command(TARGET test POST_BUILD
       COMMAND $<TARGET_FILE:${target}> --gtest_output=xml:gtests-${target}.xml)
endfunction()

# Build gmock
set(gmock_build_tests           OFF CACHE BOOL "")
set(gtest_build_samples         OFF CACHE BOOL "")
set(gtest_build_tests           OFF CACHE BOOL "")
set(gtest_disable_pthreads      OFF CACHE BOOL "")
set(gtest_force_shared_crt      ON  CACHE BOOL "")
set(gtest_hide_internal_symbols OFF CACHE BOOL "")

if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
# Target 'test'
add_custom_target(BabylonCppExtensionsUnitTests)
set_target_properties(BabylonCppExtensionsUnitTests
    PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 0)

# Tests
add_test_without_ctest(tests)
endif()

endif(OPTION_BUILD_TESTS AND EXISTS ${TESTS_PATH})

# ============================================================================ #
#                       Deployment                                             #
# ============================================================================ #

# 'make install' to the correct location
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
