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

# Header files
file(GLOB ACTIONS_HDR_FILES         ${INCLUDE_PATH}/actions/*.h
                                    ${INCLUDE_PATH}/actions/conditions/*.h
                                    ${INCLUDE_PATH}/actions/directactions/*.h
                                    ${INCLUDE_PATH}/actions/directaudioactions/*.h
                                    ${INCLUDE_PATH}/actions/valueactions/*.h)
file(GLOB ANIMATIONS_HDR_FILES      ${INCLUDE_PATH}/animations/*.h
                                    ${INCLUDE_PATH}/animations/easing/*.h)
file(GLOB AUDIO_HDR_FILES           ${INCLUDE_PATH}/audio/*.h)
file(GLOB BEHAVIORS_HDR_FILES       ${INCLUDE_PATH}/behaviors/*.h
                                    ${INCLUDE_PATH}/behaviors/cameras/*.h
                                    ${INCLUDE_PATH}/behaviors/meshes/*.h)
file(GLOB BONES_HDR_FILES           ${INCLUDE_PATH}/bones/*.h)
file(GLOB CAMERAS_HDR_FILES         ${INCLUDE_PATH}/cameras/*.h
                                    ${INCLUDE_PATH}/cameras/inputs/*.h
                                    ${INCLUDE_PATH}/cameras/stereoscopic/*.h
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
file(GLOB ENGINES_HDR_FILES         ${INCLUDE_PATH}/engines/*.h
                                    ${INCLUDE_PATH}/engines/processors/*.h
                                    ${INCLUDE_PATH}/engines/processors/expressions/*.h
                                    ${INCLUDE_PATH}/engines/processors/expressions/operators/*.h
                                    ${INCLUDE_PATH}/engines/webgl/*.h)
file(GLOB EVENTS_HDR_FILES          ${INCLUDE_PATH}/events/*.h)
file(GLOB GAMEPADS_HDR_FILES        ${INCLUDE_PATH}/gamepads/*.h
                                    ${INCLUDE_PATH}/gamepads/controllers/*.h)
file(GLOB GIZMOS_HDR_FILES          ${INCLUDE_PATH}/gizmos/*.h)
file(GLOB HELPERS_HDR_FILES         ${INCLUDE_PATH}/helpers/*.h)
file(GLOB INPUTS_HDR_FILES          ${INCLUDE_PATH}/inputs/*.h)
file(GLOB INSTRUMENTATION_HDR_FILES ${INCLUDE_PATH}/instrumentation/*.h)
file(GLOB INTERFACES_HDR_FILES      ${INCLUDE_PATH}/interfaces/*.h)
file(GLOB LAYERS_HDR_FILES          ${INCLUDE_PATH}/layers/*.h)
file(GLOB LENSFLARES_HDR_FILES      ${INCLUDE_PATH}/lensflares/*.h)
file(GLOB LIGHTS_HDR_FILES          ${INCLUDE_PATH}/lights/*.h
                                    ${INCLUDE_PATH}/lights/shadows/*.h)
file(GLOB LOADING_HDR_FILES         ${INCLUDE_PATH}/loading/*.h
                                    ${INCLUDE_PATH}/loading/plugins/babylon/*.h)
file(GLOB MATERIALS_HDR_FILES       ${INCLUDE_PATH}/materials/*.h
                                    ${INCLUDE_PATH}/materials/background/*.h
                                    ${INCLUDE_PATH}/materials/node/*.h
                                    ${INCLUDE_PATH}/materials/node/blocks/*.h
                                    ${INCLUDE_PATH}/materials/node/blocks/dual/*.h
                                    ${INCLUDE_PATH}/materials/node/blocks/fragment/*.h
                                    ${INCLUDE_PATH}/materials/node/blocks/input/*.h
                                    ${INCLUDE_PATH}/materials/node/blocks/vertex/*.h
                                    ${INCLUDE_PATH}/materials/node/enums/*.h
                                    ${INCLUDE_PATH}/materials/node/optimizers/*.h
                                    ${INCLUDE_PATH}/materials/pbr/*.h
                                    ${INCLUDE_PATH}/materials/textures/*.h
                                    ${INCLUDE_PATH}/materials/textures/loaders/*.h
                                    ${INCLUDE_PATH}/materials/textures/procedurals/*.h)
file(GLOB MATHS_HDR_FILES           ${INCLUDE_PATH}/maths/*.h)
file(GLOB MESHES_HDR_FILES          ${INCLUDE_PATH}/meshes/*.h
                                    ${INCLUDE_PATH}/meshes/builders/*.h
                                    ${INCLUDE_PATH}/meshes/csg/*.h
                                    ${INCLUDE_PATH}/meshes/polygonmesh/*.h
                                    ${INCLUDE_PATH}/meshes/simplification/*.h
                                    ${INCLUDE_PATH}/meshes/webgl/*.h)
file(GLOB MISC_HDR_FILES            ${INCLUDE_PATH}/misc/*.h
                                    ${INCLUDE_PATH}/misc/highdynamicrange/*.h
                                    ${INCLUDE_PATH}/misc/interfaces/*.h
                                    ${INCLUDE_PATH}/misc/optimization/*.h)
file(GLOB MORPH_HDR_FILES           ${INCLUDE_PATH}/morph/*.h)
file(GLOB NAVIGATION_HDR_FILES      ${INCLUDE_PATH}/navigation/*.h)
file(GLOB PARTICLES_HDR_FILES       ${INCLUDE_PATH}/particles/*.h
                                    ${INCLUDE_PATH}/particles/emittertypes/*.h)
file(GLOB PHYSICS_HDR_FILES         ${INCLUDE_PATH}/physics/*.h
                                    ${INCLUDE_PATH}/physics/helper/*.h
                                    ${INCLUDE_PATH}/physics/joint/*.h
                                    ${INCLUDE_PATH}/physics/plugins/*.h)
file(GLOB POSTPROCESSES_HDR_FILES   ${INCLUDE_PATH}/postprocesses/*.h
                                    ${INCLUDE_PATH}/postprocesses/renderpipeline/*.h
                                    ${INCLUDE_PATH}/postprocesses/renderpipeline/pipelines/*.h)
file(GLOB PROBES_HDR_FILES          ${INCLUDE_PATH}/probes/*.h)
file(GLOB RENDERING_HDR_FILES       ${INCLUDE_PATH}/rendering/*.h)
file(GLOB SHADERS_HDR_FILES         ${INCLUDE_PATH}/shaders/*.h
                                    ${INCLUDE_PATH}/shaders/shadersinclude/*.h)
file(GLOB SPRITES_HDR_FILES         ${INCLUDE_PATH}/sprites/*.h)
file(GLOB STATES_HDR_FILES          ${INCLUDE_PATH}/states/*.h)
file(GLOB UTILS_HDR_FILES           ${INCLUDE_PATH}/utils/*.h)

if (UNIX)
    set(CORE_HDR_FILES ${CORE_HDR_FILES} ${INCLUDE_PATH}/core/filesystem/filesystem_unix.h
                                         ${INCLUDE_PATH}/core/profiling/timer_unix.h)
endif (UNIX)

if (WIN32)
    set(CORE_HDR_FILES ${CORE_HDR_FILES} ${INCLUDE_PATH}/core/filesystem/filesystem_win32.h
                                         ${INCLUDE_PATH}/core/profiling/timer_win32.h)
endif (WIN32)

if (OPTION_ENABLE_SIMD)
    set(MATH_HDR_FILES ${MATH_HDR_FILES} ${INCLUDE_PATH}/math/simd/*.h)
endif (OPTION_ENABLE_SIMD)

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
    ${ENGINES_HDR_FILES}
    ${EVENTS_HDR_FILES}
    ${GAMEPADS_HDR_FILES}
    ${GIZMOS_HDR_FILES}
    ${HELPERS_HDR_FILES}
    ${INPUTS_HDR_FILES}
    ${INSTRUMENTATION_HDR_FILES}
    ${INTERFACES_HDR_FILES}
    ${LAYERS_HDR_FILES}
    ${LENSFLARES_HDR_FILES}
    ${LIGHTS_HDR_FILES}
    ${LOADING_HDR_FILES}
    ${MATERIALS_HDR_FILES}
    ${MATHS_HDR_FILES}
    ${MESHES_HDR_FILES}
    ${MISC_HDR_FILES}
    ${MORPH_HDR_FILES}
    ${NAVIGATION_HDR_FILES}
    ${PARTICLES_HDR_FILES}
    ${PHYSICS_HDR_FILES}
    ${POSTPROCESSES_HDR_FILES}
    ${PROBES_HDR_FILES}
    ${RENDERING_HDR_FILES}
    ${SHADERS_HDR_FILES}
    ${SPRITES_HDR_FILES}
    ${STATES_HDR_FILES}
    ${UTILS_HDR_FILES}
)

# Source files
file(GLOB ACTIONS_SRC_FILES         ${SOURCE_PATH}/actions/*.cpp
                                    ${SOURCE_PATH}/actions/conditions/*.cpp
                                    ${SOURCE_PATH}/actions/directactions/*.cpp
                                    ${SOURCE_PATH}/actions/directaudioactions/*.cpp
                                    ${SOURCE_PATH}/actions/valueactions/*.cpp)
file(GLOB ANIMATIONS_SRC_FILES      ${SOURCE_PATH}/animations/*.cpp
                                    ${SOURCE_PATH}/animations/easing/*.cpp)
file(GLOB AUDIO_SRC_FILES           ${SOURCE_PATH}/audio/*.cpp)
file(GLOB BEHAVIORS_SRC_FILES       ${SOURCE_PATH}/behaviors/*.cpp
                                    ${SOURCE_PATH}/behaviors/cameras/*.cpp
                                    ${SOURCE_PATH}/behaviors/meshes/*.cpp)
file(GLOB BONES_SRC_FILES           ${SOURCE_PATH}/bones/*.cpp)
file(GLOB CAMERAS_SRC_FILES         ${SOURCE_PATH}/cameras/*.cpp
                                    ${SOURCE_PATH}/cameras/inputs/*.cpp
                                    ${SOURCE_PATH}/cameras/stereoscopic/*.cpp
                                    ${SOURCE_PATH}/cameras/vr/*.cpp)
file(GLOB COLLISIONS_SRC_FILES      ${SOURCE_PATH}/collisions/*.cpp)
file(GLOB COMMON_SRC_FILES          ${SOURCE_PATH}/*.cpp)
file(GLOB CORE_SRC_FILES            ${SOURCE_PATH}/core/*.cpp
                                    ${SOURCE_PATH}/core/logging/*.cpp)
file(GLOB CULLING_SRC_FILES         ${SOURCE_PATH}/culling/*.cpp
                                    ${SOURCE_PATH}/culling/octrees/*.cpp)
file(GLOB DEBUG_SRC_FILES           ${SOURCE_PATH}/debug/*.cpp)
file(GLOB ENGINES_SRC_FILES         ${SOURCE_PATH}/engines/*.cpp
                                    ${SOURCE_PATH}/engines/processors/*.cpp
                                    ${SOURCE_PATH}/engines/processors/expressions/*.cpp
                                    ${SOURCE_PATH}/engines/processors/expressions/operators/*.cpp
                                    ${SOURCE_PATH}/engines/webgl/*.cpp)
file(GLOB EVENTS_SRC_FILES          ${SOURCE_PATH}/events/*.cpp)
file(GLOB GAMEPADS_SRC_FILES        ${SOURCE_PATH}/gamepads/*.cpp
                                    ${SOURCE_PATH}/gamepads/controllers/*.cpp)
file(GLOB GIZMOS_SRC_FILES          ${SOURCE_PATH}/gizmos/*.cpp)
file(GLOB HELPERS_SRC_FILES         ${SOURCE_PATH}/helpers/*.cpp)
file(GLOB INPUTS_SRC_FILES          ${SOURCE_PATH}/inputs/*.cpp)
file(GLOB INSTRUMENTATION_SRC_FILES ${SOURCE_PATH}/instrumentation/*.cpp)
file(GLOB INTERFACES_SRC_FILES      ${SOURCE_PATH}/interfaces/*.cpp)
file(GLOB LAYERS_SRC_FILES          ${SOURCE_PATH}/layers/*.cpp)
file(GLOB LENSFLARES_SRC_FILES      ${SOURCE_PATH}/lensflares/*.cpp)
file(GLOB LIGHTS_SRC_FILES          ${SOURCE_PATH}/lights/*.cpp
                                    ${SOURCE_PATH}/lights/shadows/*.cpp)
file(GLOB LOADING_SRC_FILES         ${SOURCE_PATH}/loading/*.cpp
                                    ${SOURCE_PATH}/loading/plugins/babylon/*.cpp)
file(GLOB MATERIALS_SRC_FILES       ${SOURCE_PATH}/materials/*.cpp
                                    ${SOURCE_PATH}/materials/background/*.cpp
                                    ${SOURCE_PATH}/materials/node/*.cpp
                                    ${SOURCE_PATH}/materials/node/blocks/*.cpp
                                    ${SOURCE_PATH}/materials/node/blocks/dual/*.cpp
                                    ${SOURCE_PATH}/materials/node/blocks/fragment/*.cpp
                                    ${SOURCE_PATH}/materials/node/blocks/input/*.cpp
                                    ${SOURCE_PATH}/materials/node/blocks/vertex/*.cpp
                                    ${SOURCE_PATH}/materials/node/optimizers/*.cpp
                                    ${SOURCE_PATH}/materials/pbr/*.cpp
                                    ${SOURCE_PATH}/materials/textures/*.cpp
                                    ${SOURCE_PATH}/materials/textures/loaders/*.cpp
                                    ${SOURCE_PATH}/materials/textures/procedurals/*.cpp)
file(GLOB MATHS_SRC_FILES           ${SOURCE_PATH}/maths/*.cpp)
file(GLOB MESHES_SRC_FILES          ${SOURCE_PATH}/meshes/*.cpp
                                    ${SOURCE_PATH}/meshes/builders/*.cpp
                                    ${SOURCE_PATH}/meshes/csg/*.cpp
                                    ${SOURCE_PATH}/meshes/polygonmesh/*.cpp
                                    ${SOURCE_PATH}/meshes/simplification/*.cpp
                                    ${SOURCE_PATH}/meshes/webgl/*.cpp)
file(GLOB MISC_SRC_FILES            ${SOURCE_PATH}/misc/*.cpp
                                    ${SOURCE_PATH}/misc/highdynamicrange/*.cpp
                                    ${SOURCE_PATH}/misc/optimization/*.cpp)
file(GLOB MORPH_SRC_FILES           ${SOURCE_PATH}/morph/*.cpp)
file(GLOB PARTICLES_SRC_FILES       ${SOURCE_PATH}/particles/*.cpp
                                    ${SOURCE_PATH}/particles/emittertypes/*.cpp)
file(GLOB PHYSICS_SRC_FILES         ${SOURCE_PATH}/physics/*.cpp
                                    ${SOURCE_PATH}/physics/helper/*.cpp
                                    ${SOURCE_PATH}/physics/joint/*.cpp
                                    ${SOURCE_PATH}/physics/plugins/*.cpp)
file(GLOB POSTPROCESSES_SRC_FILES   ${SOURCE_PATH}/postprocesses/*.cpp
                                    ${SOURCE_PATH}/postprocesses/renderpipeline/*.cpp
                                    ${SOURCE_PATH}/postprocesses/renderpipeline/pipelines/*.cpp)
file(GLOB PROBES_SRC_FILES          ${SOURCE_PATH}/probes/*.cpp)
file(GLOB RENDERING_SRC_FILES       ${SOURCE_PATH}/rendering/*.cpp)
file(GLOB SPRITES_SRC_FILES         ${SOURCE_PATH}/sprites/*.cpp)
file(GLOB STATES_SRC_FILES          ${SOURCE_PATH}/states/*.cpp)
file(GLOB UTILS_SRC_FILES           ${SOURCE_PATH}/utils/*.cpp)

if (UNIX)
    set(CORE_SRC_FILES ${CORE_SRC_FILES} ${SOURCE_PATH}/core/profiling/timer_unix.cpp)
endif (UNIX)

if (WIN32)
    set(CORE_SRC_FILES ${CORE_SRC_FILES} ${SOURCE_PATH}/core/profiling/timer_win32.cpp)
endif (WIN32)

if (OPTION_ENABLE_SIMD)
    set(MATH_SRC_FILES ${MATH_SRC_FILES} ${SOURCE_PATH}/math/simd/*.cpp)
endif (OPTION_ENABLE_SIMD)

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
    ${ENGINES_SRC_FILES}
    ${EVENTS_SRC_FILES}
    ${GAMEPADS_SRC_FILES}
    ${GIZMOS_SRC_FILES}
    ${HELPERS_SRC_FILES}
    ${INPUTS_SRC_FILES}
    ${INSTRUMENTATION_SRC_FILES}
    ${INTERFACES_SRC_FILES}
    ${LAYERS_SRC_FILES}
    ${LENSFLARES_SRC_FILES}
    ${LIGHTS_SRC_FILES}
    ${LOADING_SRC_FILES}
    ${MATERIALS_SRC_FILES}
    ${MATHS_SRC_FILES}
    ${MESHES_SRC_FILES}
    ${MISC_SRC_FILES}
    ${MORPH_SRC_FILES}
    ${PARTICLES_SRC_FILES}
    ${PHYSICS_SRC_FILES}
    ${POSTPROCESSES_SRC_FILES}
    ${PROBES_SRC_FILES}
    ${RENDERING_SRC_FILES}
    ${SHADERS_SRC_FILES}
    ${SPRITES_SRC_FILES}
    ${STATES_SRC_FILES}
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
babylon_target_clang_tidy(${TARGET})

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
    PUBLIC SYSTEM
    ${STB_IMAGE_INCLUDE_DIR}
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
endif()
