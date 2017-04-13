# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET OimoCpp)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "Lib ${TARGET}")

# Namespace
set(OIMO_NAMESPACE oimo)

# Set API export file and macro
string(TOUPPER ${OIMO_NAMESPACE} TARGET_UPPER)
set(FEATURE_FILE "include/${OIMO_NAMESPACE}/${OIMO_NAMESPACE}_features.h")
set(EXPORT_FILE  "include/${OIMO_NAMESPACE}/${OIMO_NAMESPACE}_api.h")
set(EXPORT_MACRO "${TARGET_UPPER}_SHARED_EXPORT")

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)

# Meta information about the project
set(META_PROJECT_NAME        "OimoCpp")
set(META_PROJECT_DESCRIPTION "")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "1")
set(META_VERSION_MINOR       "2")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${OIMO_NAMESPACE}/${OIMO_NAMESPACE}_version.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include and Source path
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${OIMO_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Header files
file(GLOB COLLISION_HDR_FILES       ${INCLUDE_PATH}/collision/broadphase/*.h
                                    ${INCLUDE_PATH}/collision/broadphase/dbvt/*.h
                                    ${INCLUDE_PATH}/collision/broadphase/sap/*.h
                                    ${INCLUDE_PATH}/collision/narrowphase/*.h
                                    ${INCLUDE_PATH}/collision/shape/*.h)
file(GLOB COMMON_HDR_FILES          ${INCLUDE_PATH}/*.h)
file(GLOB CONSTRAINT_HDR_FILES      ${INCLUDE_PATH}/constraint/*.h
                                    ${INCLUDE_PATH}/constraint/contact/*.h
                                    ${INCLUDE_PATH}/constraint/joint/*.h
                                    ${INCLUDE_PATH}/constraint/joint/base/*.h)
file(GLOB DYNAMICS_HDR_FILES        ${INCLUDE_PATH}/dynamics/*.h)
file(GLOB MATH_HDR_FILES            ${INCLUDE_PATH}/math/*.h)
file(GLOB SHAPE_HDR_FILES           ${INCLUDE_PATH}/shape/*.h)
file(GLOB UTIL_HDR_FILES            ${INCLUDE_PATH}/util/*.h)

set(OIMO_HEADERS
    ${COLLISION_HDR_FILES}
    ${COMMON_HDR_FILES}
    ${CONSTRAINT_HDR_FILES}
    ${DYNAMICS_HDR_FILES}
    ${MATH_HDR_FILES}
    ${SHAPE_HDR_FILES}
    ${UTIL_HDR_FILES}
)

# Source files
file(GLOB COLLISION_SRC_FILES       ${SOURCE_PATH}/collision/broadphase/*.cpp
                                    ${SOURCE_PATH}/collision/broadphase/dbvt/*.cpp
                                    ${SOURCE_PATH}/collision/broadphase/sap/*.cpp
                                    ${SOURCE_PATH}/collision/narrowphase/*.cpp
                                    ${SOURCE_PATH}/collision/shape/*.cpp)
file(GLOB CONSTRAINT_SRC_FILES      ${SOURCE_PATH}/constraint/*.cpp
                                    ${SOURCE_PATH}/constraint/contact/*.cpp
                                    ${SOURCE_PATH}/constraint/joint/*.cpp
                                    ${SOURCE_PATH}/constraint/joint/base/*.cpp)
file(GLOB DYNAMICS_SRC_FILES        ${SOURCE_PATH}/dynamics/*.cpp)
file(GLOB MATH_SRC_FILES            ${SOURCE_PATH}/math/*.cpp)
file(GLOB SHAPE_SRC_FILES           ${SOURCE_PATH}/shape/*.cpp)
file(GLOB UTIL_SRC_FILES            ${SOURCE_PATH}/util/*.cpp)

set(OIMO_SOURCES
    ${COLLISION_SRC_FILES}
    ${CONSTRAINT_SRC_FILES}
    ${DYNAMICS_SRC_FILES}
    ${MATH_SRC_FILES}
    ${SHAPE_SRC_FILES}
    ${UTIL_SRC_FILES}
)

# Group source files
set(HEADER_GROUP "Header Files (API)")
set(SOURCE_GROUP "Source Files")
source_group_by_path(${INCLUDE_PATH} "\\\\.h$|\\\\.hpp$"
    ${HEADER_GROUP} ${OIMO_HEADERS})
source_group_by_path(${SOURCE_PATH}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${SOURCE_GROUP} ${OIMO_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${OIMO_SOURCES}
    ${OIMO_HEADERS}
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
    PUBLIC
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
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
#                       Deployment                                             #
# ============================================================================ #

# 'make install' to the correct location
install(TARGETS ${TARGET}
    EXPORT   "${TARGET}-export"                    COMPONENT dev
    ARCHIVE  DESTINATION ${ARCHIVE_OUTPUT_PATH}    COMPONENT dev
    RUNTIME  DESTINATION ${EXECUTABLE_OUTPUT_PATH} COMPONENT runtime
    LIBRARY  DESTINATION ${LIBRARY_OUTPUT_PATH}    COMPONENT runtime
)