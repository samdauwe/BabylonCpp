# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET BabylonImGui)

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
set(META_PROJECT_NAME        "imgui_babylon")
set(META_PROJECT_DESCRIPTION "Inspector and imgui scene widget for BabylonCpp")
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
#                       Sources                                                #
# ============================================================================ #

# Include, Source
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Header files
file(GLOB_RECURSE BABYLON_INSPECTOR_HEADERS ${INCLUDE_PATH}/*.h)
file(GLOB_RECURSE BABYLON_INSPECTOR_SOURCES ${SOURCE_PATH}/*.cpp)

source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${BABYLON_INSPECTOR_HEADERS} ${BABYLON_INSPECTOR_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    STATIC
    ${BABYLON_INSPECTOR_SOURCES}
    ${BABYLON_INSPECTOR_HEADERS}
    Readme.md
)
babylon_target_clang_tidy(${TARGET})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/../MaterialsLibrary/include
    PUBLIC
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    BabylonCpp
    MaterialsLibrary
    Samples
    imgui
    imgui_utils
)

if (APPLE AND NOT EMSCRIPTEN) # Special case for Apple OpenGL framework
    target_link_libraries(${TARGET} PRIVATE "-framework OpenGL")
endif()

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${TARGET_UPPER}_STATIC_DEFINE>
)

if (BABYLON_BUILD_PLAYGROUND)
    target_compile_definitions(${TARGET} PRIVATE -DBABYLON_BUILD_PLAYGROUND)
endif()

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
#                       Deployment                                             #
# ============================================================================ #

set(HACK_DISABLE_INSTALL ON)

if (NOT HACK_DISABLE_INSTALL AND NOT BABYLON_DISABLE_INSTALL)
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
endif()

# Copy asset directories
## - Fonts
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/fonts")
## - Screenshots
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/screenshots")
