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
set(META_PROJECT_NAME        "BabylonCpp")
set(META_PROJECT_DESCRIPTION "")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "3")
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

# Include, Source and Tests path
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TESTS_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Header files
file(GLOB ACTIONS_HDR_FILES         ${INCLUDE_PATH}/actions/*.h
                                    ${INCLUDE_PATH}/actions/conditions/*.h
                                    ${INCLUDE_PATH}/actions/directactions/*.h
                                    ${INCLUDE_PATH}/actions/valueactions/*.h)
file(GLOB ANIMATIONS_HDR_FILES      ${INCLUDE_PATH}/animations/*.h
                                    ${INCLUDE_PATH}/animations/easing/*.h)
file(GLOB AUDIO_HDR_FILES           ${INCLUDE_PATH}/audio/*.h)
file(GLOB BEHAVIORS_HDR_FILES       ${INCLUDE_PATH}/behaviors/*.h
                                    ${INCLUDE_PATH}/behaviors/cameras/*.h)
file(GLOB BONES_HDR_FILES           ${INCLUDE_PATH}/bones/*.h)
file(GLOB CAMERAS_HDR_FILES         ${INCLUDE_PATH}/cameras/*.h
                                    ${INCLUDE_PATH}/cameras/inputs/*.h
                                    ${INCLUDE_PATH}/cameras/vr/*.h)
file(GLOB COLLISIONS_HDR_FILES      ${INCLUDE_PATH}/collisions/*.h)
file(GLOB COMMON_HDR_FILES          ${INCLUDE_PATH}/*.h)
file(GLOB CORE_HDR_FILES            ${INCLUDE_PATH}/core/*.h
                                    ${INCLUDE_PATH}/core/delegates/*.h
                                    ${INCLUDE_PATH}/core/filesystem/filesystem_common.h
                                    ${INCLUDE_PATH}/core/logging/*.h)
file(GLOB CULLING_HDR_FILES         ${INCLUDE_PATH}/culling/*.h
                                    ${INCLUDE_PATH}/culling/octrees/*.h)
file(GLOB DEBUG_HDR_FILES           ${INCLUDE_PATH}/debug/*.h)
file(GLOB ENGINE_HDR_FILES          ${INCLUDE_PATH}/engine/*.h)
file(GLOB GAMEPAD_HDR_FILES         ${INCLUDE_PATH}/gamepad/*.h
                                    ${INCLUDE_PATH}/gamepad/controllers/*.h)
file(GLOB INTERFACES_HDR_FILES      ${INCLUDE_PATH}/interfaces/.h)
file(GLOB LAYER_HDR_FILES           ${INCLUDE_PATH}/layer/*.h)
file(GLOB LENSFLARE_HDR_FILES       ${INCLUDE_PATH}/lensflare/*.h)
file(GLOB LIGHTS_HDR_FILES          ${INCLUDE_PATH}/lights/*.h
                                    ${INCLUDE_PATH}/lights/shadows/*.h)
file(GLOB LOADING_HDR_FILES         ${INCLUDE_PATH}/loading/*.h
                                    ${INCLUDE_PATH}/loading/plugins/babylon/*.h)
file(GLOB MATERIALS_HDR_FILES       ${INCLUDE_PATH}/materials/*.h
                                    ${INCLUDE_PATH}/materials/pbr/*.h
                                    ${INCLUDE_PATH}/materials/textures/*.h
                                    ${INCLUDE_PATH}/materials/textures/procedurals/*.h)
file(GLOB MATH_HDR_FILES            ${INCLUDE_PATH}/math/*.h
                                    ${INCLUDE_PATH}/math/simd/*.h)
file(GLOB MESH_HDR_FILES            ${INCLUDE_PATH}/mesh/*.h
                                    ${INCLUDE_PATH}/mesh/csg/*.h
                                    ${INCLUDE_PATH}/mesh/geometryprimitives/*.h
                                    ${INCLUDE_PATH}/mesh/polygonmesh/*.h
                                    ${INCLUDE_PATH}/mesh/simplification/*.h)
file(GLOB MORPH_HDR_FILES           ${INCLUDE_PATH}/morph/*.h)
file(GLOB PARTICLES_HDR_FILES       ${INCLUDE_PATH}/particles/*.h)
file(GLOB PHYSICS_HDR_FILES         ${INCLUDE_PATH}/physics/*.h
                                    ${INCLUDE_PATH}/physics/joint/*.h
                                    ${INCLUDE_PATH}/physics/plugins/*.h)
file(GLOB POSTPROCESS_HDR_FILES     ${INCLUDE_PATH}/postprocess/*.h
                                    ${INCLUDE_PATH}/postprocess/renderpipeline/*.h
                                    ${INCLUDE_PATH}/postprocess/renderpipeline/pipelines/*.h)
file(GLOB PROBES_HDR_FILES          ${INCLUDE_PATH}/probes/*.h)
file(GLOB RENDERING_HDR_FILES       ${INCLUDE_PATH}/rendering/*.h)
file(GLOB SHADERS_HDR_FILES         ${INCLUDE_PATH}/shaders/*.h
                                    ${INCLUDE_PATH}/shaders/shadersinclude/*.h)
file(GLOB SPRITES_HDR_FILES         ${INCLUDE_PATH}/sprites/*.h)
file(GLOB STATES_HDR_FILES          ${INCLUDE_PATH}/states/*.h)
file(GLOB TOOLS_HDR_FILES           ${INCLUDE_PATH}/tools/*.h
                                    ${INCLUDE_PATH}/tools/hdr/*.h
                                    ${INCLUDE_PATH}/tools/optimization/*.h)
file(GLOB UTILS_HDR_FILES           ${INCLUDE_PATH}/utils/*.h
                                    ${INCLUDE_PATH}/utils/utf8/*.h)

if (UNIX)
    set(CORE_HDR_FILES ${CORE_HDR_FILES} ${INCLUDE_PATH}/core/filesystem/filesystem_unix.h
                                         ${INCLUDE_PATH}/core/profiling/timer_unix.h)
endif (UNIX)

if (WIN32)
    set(CORE_HDR_FILES ${CORE_HDR_FILES} ${INCLUDE_PATH}/core/profiling/timer_win32.h)
endif (WIN32)

set(BABYLON_HEADERS
    ${ACTIONS_HDR_FILES}
    ${ANIMATIONS_HDR_FILES}
    ${AUDIO_HDR_FILES}
    ${BEHAVIORS_HDR_FILES}
    ${BONES_HDR_FILES}
    ${CAMERAS_HDR_FILES}
    ${COLLISIONS_HDR_FILES}
    ${COMMON_HDR_FILES}
    ${CORE_HDR_FILES}
    ${CULLING_HDR_FILES}
    ${DEBUG_HDR_FILES}
    ${ENGINE_HDR_FILES}
    ${GAMEPAD_HDR_FILES}
    ${INTERFACES_HDR_FILES}
    ${LAYER_HDR_FILES}
    ${LENSFLARE_HDR_FILES}
    ${LIGHTS_HDR_FILES}
    ${LOADING_HDR_FILES}
    ${MATERIALS_HDR_FILES}
    ${MATH_HDR_FILES}
    ${MESH_HDR_FILES}
    ${MORPH_HDR_FILES}
    ${PARTICLES_HDR_FILES}
    ${PHYSICS_HDR_FILES}
    ${POSTPROCESS_HDR_FILES}
    ${PROBES_HDR_FILES}
    ${RENDERING_HDR_FILES}
    ${SHADERS_HDR_FILES}
    ${SPRITES_HDR_FILES}
    ${STATES_HDR_FILES}
    ${TOOLS_HDR_FILES}
    ${UTILS_HDR_FILES}
)

# Source files
file(GLOB ACTIONS_SRC_FILES         ${SOURCE_PATH}/actions/*.cpp
                                    ${SOURCE_PATH}/actions/conditions/*.cpp
                                    ${SOURCE_PATH}/actions/directactions/*.cpp
                                    ${SOURCE_PATH}/actions/valueactions/*.cpp)
file(GLOB ANIMATIONS_SRC_FILES      ${SOURCE_PATH}/animations/*.cpp
                                    ${SOURCE_PATH}/animations/easing/*.cpp)
file(GLOB AUDIO_SRC_FILES           ${SOURCE_PATH}/audio/*.cpp)
file(GLOB BEHAVIORS_SRC_FILES       ${SOURCE_PATH}/behaviors/*.cpp
                                    ${SOURCE_PATH}/behaviors/cameras/*.cpp)
file(GLOB BONES_SRC_FILES           ${SOURCE_PATH}/bones/*.cpp)
file(GLOB CAMERAS_SRC_FILES         ${SOURCE_PATH}/cameras/*.cpp
                                    ${SOURCE_PATH}/cameras/inputs/*.cpp
                                    ${SOURCE_PATH}/cameras/vr/*.cpp)
file(GLOB COLLISIONS_SRC_FILES      ${SOURCE_PATH}/collisions/*.cpp)
file(GLOB COMMON_SRC_FILES          ${SOURCE_PATH}/*.cpp)
file(GLOB CORE_SRC_FILES            ${SOURCE_PATH}/core/*.cpp
                                    ${SOURCE_PATH}/core/logging/*.cpp)
file(GLOB CULLING_SRC_FILES         ${SOURCE_PATH}/culling/*.cpp
                                    ${SOURCE_PATH}/culling/octrees/*.cpp)
file(GLOB DEBUG_SRC_FILES           ${SOURCE_PATH}/debug/*.cpp)
file(GLOB ENGINE_SRC_FILES          ${SOURCE_PATH}/engine/*.cpp)
file(GLOB GAMEPAD_SRC_FILES         ${SOURCE_PATH}/gamepad/*.cpp
                                    ${SOURCE_PATH}/gamepad/controllers/*.cpp)
file(GLOB INTERFACES_SRC_FILES      ${SOURCE_PATH}/interfaces/*.cpp)
file(GLOB LAYER_SRC_FILES           ${SOURCE_PATH}/layer/*.cpp)
file(GLOB LENSFLARE_SRC_FILES       ${SOURCE_PATH}/lensflare/*.cpp)
file(GLOB LIGHTS_SRC_FILES          ${SOURCE_PATH}/lights/*.cpp
                                    ${SOURCE_PATH}/lights/shadows/*.cpp)
file(GLOB LOADING_SRC_FILES         ${SOURCE_PATH}/loading/*.cpp
                                    ${SOURCE_PATH}/loading/plugins/babylon/*.cpp)
file(GLOB MATERIALS_SRC_FILES       ${SOURCE_PATH}/materials/*.cpp
                                    ${SOURCE_PATH}/materials/pbr/*.cpp
                                    ${SOURCE_PATH}/materials/textures/*.cpp
                                    ${SOURCE_PATH}/materials/textures/procedurals/*.cpp)
file(GLOB MATH_SRC_FILES            ${SOURCE_PATH}/math/*.cpp
                                    ${SOURCE_PATH}/math/simd/*.cpp)
file(GLOB MESH_SRC_FILES            ${SOURCE_PATH}/mesh/*.cpp
                                    ${SOURCE_PATH}/mesh/csg/*.cpp
                                    ${SOURCE_PATH}/mesh/geometryprimitives/*.cpp
                                    ${SOURCE_PATH}/mesh/polygonmesh/*.cpp
                                    ${SOURCE_PATH}/mesh/simplification/*.cpp)
file(GLOB MORPH_SRC_FILES           ${SOURCE_PATH}/morph/*.cpp)
file(GLOB PARTICLES_SRC_FILES       ${SOURCE_PATH}/particles/*.cpp)
file(GLOB PHYSICS_SRC_FILES         ${SOURCE_PATH}/physics/*.cpp
                                    ${SOURCE_PATH}/physics/joint/*.cpp
                                    ${SOURCE_PATH}/physics/plugins/*.cpp)
file(GLOB POSTPROCESS_SRC_FILES     ${SOURCE_PATH}/postprocess/*.cpp
                                    ${SOURCE_PATH}/postprocess/renderpipeline/*.cpp
                                    ${SOURCE_PATH}/postprocess/renderpipeline/pipelines/*.cpp)
file(GLOB PROBES_SRC_FILES          ${SOURCE_PATH}/probes/*.cpp)
file(GLOB RENDERING_SRC_FILES       ${SOURCE_PATH}/rendering/*.cpp)
file(GLOB SPRITES_SRC_FILES         ${SOURCE_PATH}/sprites/*.cpp)
file(GLOB STATES_SRC_FILES          ${SOURCE_PATH}/states/*.cpp)
file(GLOB TOOLS_SRC_FILES           ${SOURCE_PATH}/tools/*.cpp
                                    ${SOURCE_PATH}/tools/hdr/*.cpp
                                    ${SOURCE_PATH}/tools/optimization/*.cpp)
file(GLOB UTILS_SRC_FILES           ${SOURCE_PATH}/utils/*.cpp)

if (UNIX)
    set(CORE_SRC_FILES ${CORE_SRC_FILES} ${SOURCE_PATH}/core/profiling/timer_unix.cpp)
endif (UNIX)

if (WIN32)
    set(CORE_SRC_FILES ${CORE_SRC_FILES} ${SOURCE_PATH}/core/profiling/timer_win32.cpp)
endif (WIN32)

set(BABYLON_SOURCES
    ${ACTIONS_SRC_FILES}
    ${ANIMATIONS_SRC_FILES}
    ${AUDIO_SRC_FILES}
    ${BEHAVIORS_SRC_FILES}
    ${BONES_SRC_FILES}
    ${CAMERAS_SRC_FILES}
    ${COLLISIONS_SRC_FILES}
    ${COMMON_SRC_FILES}
    ${CORE_SRC_FILES}
    ${CULLING_SRC_FILES}
    ${DEBUG_SRC_FILES}
    ${ENGINE_SRC_FILES}
    ${GAMEPAD_SRC_FILES}
    ${INTERFACES_SRC_FILES}
    ${LAYER_SRC_FILES}
    ${LENSFLARE_SRC_FILES}
    ${LIGHTS_SRC_FILES}
    ${LOADING_SRC_FILES}
    ${MATERIALS_SRC_FILES}
    ${MATH_SRC_FILES}
    ${MESH_SRC_FILES}
    ${MORPH_SRC_FILES}
    ${PARTICLES_SRC_FILES}
    ${PHYSICS_SRC_FILES}
    ${POSTPROCESS_SRC_FILES}
    ${PROBES_SRC_FILES}
    ${RENDERING_SRC_FILES}
    ${SHADERS_SRC_FILES}
    ${SPRITES_SRC_FILES}
    ${STATES_SRC_FILES}
    ${TOOLS_SRC_FILES}
    ${UTILS_SRC_FILES}
)

# Group source files
set(HEADER_GROUP "Header Files (API)")
set(SOURCE_GROUP "Source Files")
source_group_by_path(${INCLUDE_PATH} "\\\\.h$|\\\\.hpp$"
    ${HEADER_GROUP} ${BABYLON_HEADERS})
source_group_by_path(${SOURCE_PATH}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${SOURCE_GROUP} ${BABYLON_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_SOURCES}
    ${BABYLON_HEADERS}
)

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
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${EARCUT_HPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../OimoCpp/include
    PUBLIC
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    OimoCpp
)

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${TARGET_UPPER}_STATIC_DEFINE>
)

# Compile options
target_compile_options(${TARGET}
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

# DISABLE CMP0037: Target names should not be reserved and should match a
# validity pattern.
set_policy(CMP0037 OLD)

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

# Create interface library to link against gmock
add_library(gmock-dev INTERFACE)

target_include_directories(gmock-dev
    SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include
)

target_link_libraries(gmock-dev
    INTERFACE
    gmock
)

# Target 'tests'
add_custom_target(BabylonCppUnitTests)
set_target_properties(BabylonCppUnitTests
    PROPERTIES TEST_EXCLUDE_FROM_DEFAULT_BUILD 0)

# Tests
add_test_without_ctest(tests)

# Target 'benchmarks'
add_custom_target(BabylonCppBenchmarks)
set_target_properties(BabylonCppBenchmarks
    PROPERTIES TEST_EXCLUDE_FROM_DEFAULT_BUILD 1)

# Benchmarks
add_test_without_ctest(benchmarks)

endif(OPTION_BUILD_TESTS AND EXISTS ${TESTS_PATH})

# ============================================================================ #
#                       Deployment                                             #
# ============================================================================ #

# 'make install' to the correct location
install(TARGETS ${TARGET}
    EXPORT   "${TARGET}-export"                    COMPONENT dev
    ARCHIVE  DESTINATION ${ARCHIVE_OUTPUT_PATH}    COMPONENT dev
    RUNTIME  DESTINATION ${EXECUTABLE_OUTPUT_PATH} COMPONENT runtime
    LIBRARY  DESTINATION ${LIBRARY_OUTPUT_PATH}    COMPONENT runtime
)
