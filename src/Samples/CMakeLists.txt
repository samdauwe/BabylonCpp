# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Samples)

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
set(META_PROJECT_NAME        "Samples")
set(META_PROJECT_DESCRIPTION "")
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

# Loader plugins
if (OPTION_BUILD_LOADERS)
    # Set define for usage in source code
    add_definitions(-DWITH_LOADERS)
endif()

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source and Tests path
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}/samples")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src/samples")
set(TESTS_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Header files
file(GLOB ANIMATIONS_HDR_FILES                      ${INCLUDE_PATH}/animations/*.h
                                                    ${INCLUDE_PATH}/animations/easing/*.h)
file(GLOB CAMERAS_HDR_FILES                         ${INCLUDE_PATH}/cameras/*.h)
file(GLOB COLLISION_AND_INTERSECTIONS_HDR_FILES     ${INCLUDE_PATH}/collisionsandintersections/*.h)
file(GLOB COMMON_HDR_FILES                          ${INCLUDE_PATH}/*.h)
file(GLOB EXTENSIONS_HDR_FILES                      ${INCLUDE_PATH}/extensions/*.h
                                                    ${INCLUDE_PATH}/extensions/hexplanetgeneration/*.h
                                                    ${INCLUDE_PATH}/extensions/navigation/*.h
                                                    ${INCLUDE_PATH}/extensions/noisegeneration/*.h
                                                    ${INCLUDE_PATH}/extensions/polyhedron/*.h
                                                    ${INCLUDE_PATH}/extensions/treegenerators/*.h)
file(GLOB LIGHTS_HDR_FILES                          ${INCLUDE_PATH}/lights/*.h)
if (OPTION_BUILD_LOADERS)
    file(GLOB LOADERS_HDR_FILES                     ${INCLUDE_PATH}/loaders/*.h
                                                    ${INCLUDE_PATH}/loaders/babylon/*.h
                                                    ${INCLUDE_PATH}/loaders/gltf/*.h
                                                    ${INCLUDE_PATH}/loaders/gltf/morecomplexmodels/*.h
                                                    ${INCLUDE_PATH}/loaders/gltf/simplemodels/*.h)
else()
    file(GLOB LOADERS_HDR_FILES                     ${INCLUDE_PATH}/loaders/*.h
                                                    ${INCLUDE_PATH}/loaders/babylon/*.h)
endif()
file(GLOB MATERIALS_HDR_FILES                       ${INCLUDE_PATH}/materials/*.h
                                                    ${INCLUDE_PATH}/materials/shadermaterial/*.h)
file(GLOB MATERIALS_LIBRARY_HDR_FILES               ${INCLUDE_PATH}/materialslibrary/*.h)
file(GLOB MESHES_HDR_FILES                          ${INCLUDE_PATH}/meshes/*.h
                                                    ${INCLUDE_PATH}/meshes/polygonmesh/*.h)
file(GLOB OPTIMIZATIONS_HDR_FILES                   ${INCLUDE_PATH}/optimizations/*.h)
file(GLOB PARTICLES_HDR_FILES                       ${INCLUDE_PATH}/particles/*.h)
file(GLOB PROCEDURAL_TEXTURES_LIBRARY_HDR_FILES     ${INCLUDE_PATH}/proceduraltextureslibrary/*.h)
file(GLOB SHADOWS_HDR_FILES                         ${INCLUDE_PATH}/shadows/*.h)
file(GLOB SPECIAL_FX_HDR_FILES                      ${INCLUDE_PATH}/specialfx/*.h)
file(GLOB TEXTURES_FX_HDR_FILES                     ${INCLUDE_PATH}/textures/*.h)

set(BABYLON_SAMPLES_HEADERS
    ${ANIMATIONS_HDR_FILES}
    ${CAMERAS_HDR_FILES}
    ${COLLISION_AND_INTERSECTIONS_HDR_FILES}
    ${COMMON_HDR_FILES}
    ${EXTENSIONS_HDR_FILES}
    ${LIGHTS_HDR_FILES}
    ${LOADERS_HDR_FILES}
    ${MATERIALS_HDR_FILES}
    ${MATERIALS_LIBRARY_HDR_FILES}
    ${MESHES_HDR_FILES}
    ${OPTIMIZATIONS_HDR_FILES}
    ${PARTICLES_HDR_FILES}
    ${PROCEDURAL_TEXTURES_LIBRARY_HDR_FILES}
    ${SHADOWS_HDR_FILES}
    ${SPECIAL_FX_HDR_FILES}
    ${TEXTURES_FX_HDR_FILES}
)

# Source files
file(GLOB ANIMATIONS_SRC_FILES                      ${SOURCE_PATH}/animations/*.cpp
                                                    ${SOURCE_PATH}/animations/easing/*.cpp)
file(GLOB CAMERAS_SRC_FILES                         ${SOURCE_PATH}/cameras/*.cpp)
file(GLOB COLLISION_AND_INTERSECTIONS_SRC_FILES     ${SOURCE_PATH}/collisionsandintersections/*.cpp)
file(GLOB COMMON_SRC_FILES                          ${SOURCE_PATH}/*.cpp)
file(GLOB EXTENSIONS_SRC_FILES                      ${SOURCE_PATH}/extensions/*.cpp
                                                    ${SOURCE_PATH}/extensions/hexplanetgeneration/*.cpp
                                                    ${SOURCE_PATH}/extensions/navigation/*.cpp
                                                    ${SOURCE_PATH}/extensions/noisegeneration/*.cpp
                                                    ${SOURCE_PATH}/extensions/polyhedron/*.cpp
                                                    ${SOURCE_PATH}/extensions/treegenerators/*.cpp)
file(GLOB LIGHTS_SRC_FILES                          ${SOURCE_PATH}/lights/*.cpp)
if (OPTION_BUILD_LOADERS)
    file(GLOB LOADERS_SRC_FILES                     ${SOURCE_PATH}/loaders/*.cpp
                                                    ${SOURCE_PATH}/loaders/babylon/*.cpp
                                                    ${SOURCE_PATH}/loaders/gltf/*.cpp
                                                    ${SOURCE_PATH}/loaders/gltf/morecomplexmodels/*.cpp
                                                    ${SOURCE_PATH}/loaders/gltf/simplemodels/*.cpp)
else()
    file(GLOB LOADERS_SRC_FILES                     ${SOURCE_PATH}/loaders/*.cpp
                                                    ${SOURCE_PATH}/loaders/babylon/*.cpp)
endif()
file(GLOB MATERIALS_SRC_FILES                       ${SOURCE_PATH}/materials/*.cpp
                                                    ${SOURCE_PATH}/materials/shadermaterial/*.cpp)
file(GLOB MATERIALS_LIBRARY_SRC_FILES               ${SOURCE_PATH}/materialslibrary/*.cpp)
file(GLOB MESHES_SRC_FILES                          ${SOURCE_PATH}/meshes/*.cpp
                                                    ${SOURCE_PATH}/meshes/polygonmesh/*.cpp)
file(GLOB OPTIMIZATIONS_SRC_FILES                   ${SOURCE_PATH}/optimizations/*.cpp)
file(GLOB PARTICLES_SRC_FILES                       ${SOURCE_PATH}/particles/*.cpp)
file(GLOB PROCEDURAL_TEXTURES_LIBRARY_SRC_FILES     ${SOURCE_PATH}/proceduraltextureslibrary/*.cpp)
file(GLOB SHADOWS_SRC_FILES                         ${SOURCE_PATH}/shadows/*.cpp)
file(GLOB SPECIAL_FX_SRC_FILES                      ${SOURCE_PATH}/specialfx/*.cpp)
file(GLOB TEXTURES_FX_SRC_FILES                     ${SOURCE_PATH}/textures/*.cpp)

set(BABYLON_SAMPLES_SOURCES
    ${ANIMATIONS_SRC_FILES}
    ${CAMERAS_SRC_FILES}
    ${COLLISION_AND_INTERSECTIONS_SRC_FILES}
    ${COMMON_SRC_FILES}
    ${EXTENSIONS_SRC_FILES}
    ${LIGHTS_SRC_FILES}
    ${LOADERS_SRC_FILES}
    ${MATERIALS_SRC_FILES}
    ${MATERIALS_LIBRARY_SRC_FILES}
    ${MESHES_SRC_FILES}
    ${OPTIMIZATIONS_SRC_FILES}
    ${PARTICLES_SRC_FILES}
    ${PROCEDURAL_TEXTURES_LIBRARY_SRC_FILES}
    ${SHADOWS_SRC_FILES}
    ${SPECIAL_FX_SRC_FILES}
    ${TEXTURES_FX_SRC_FILES}
)

# Group source files
set(HEADER_GROUP "Header Files (API)")
set(SOURCE_GROUP "Source Files")
source_group_by_path(${INCLUDE_PATH} "\\\\.h$|\\\\.hpp$"
                     ${HEADER_GROUP} ${BABYLON_SAMPLES_HEADERS})
source_group_by_path(${SOURCE_PATH}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
                     ${SOURCE_GROUP} ${BABYLON_SAMPLES_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_SAMPLES_SOURCES}
    ${BABYLON_SAMPLES_HEADERS}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extensions/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../MaterialsLibrary/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ProceduralTexturesLibrary/include
    PUBLIC
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if (OPTION_BUILD_LOADERS)
    target_include_directories(${TARGET}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Loaders/include
        ${CMAKE_CURRENT_BINARY_DIR}/../Loaders/include
    )
endif()

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    PRIVATE
    BabylonCpp
    Extensions
    MaterialsLibrary
    ProceduralTexturesLibrary
)

if (OPTION_BUILD_LOADERS)
    target_link_libraries(${TARGET}
        PRIVATE
        Loaders
    )
endif()

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
    PROPERTIES  PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}"
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

# Target 'test'
add_custom_target(BabylonCppExtensionsUnitTests)
set_target_properties(BabylonCppExtensionsUnitTests
    PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 0)

# Tests
add_test_without_ctest(tests)

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

# Copy asset directories
# Loaders assets
if (OPTION_BUILD_LOADERS)
    copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/glTF-Sample-Models")
endif()
## - Scenes
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/scenes")
## - Textures
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/textures")
