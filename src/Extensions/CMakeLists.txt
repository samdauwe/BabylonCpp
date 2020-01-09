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

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Meta information about the project
set(META_PROJECT_NAME        "Extensions")
set(META_PROJECT_DESCRIPTION "Extensions for BabylonCpp")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source and Tests path
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Header files
file(GLOB_RECURSE BABYLON_EXTENSIONS_HEADERS  ${INCLUDE_PATH}/*.h)
file(GLOB_RECURSE BABYLON_EXTENSIONS_SOURCES  ${SOURCE_PATH}/*.cpp)
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${BABYLON_EXTENSIONS_HEADERS} ${BABYLON_EXTENSIONS_SOURCES})

# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_EXTENSIONS_SOURCES}
    ${BABYLON_EXTENSIONS_HEADERS}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${INCLUDE_PATH}/../recastnavigation/Detour/Include>
    $<BUILD_INTERFACE:${INCLUDE_PATH}/../recastnavigation/DetourCrowd/Include>
    $<BUILD_INTERFACE:${INCLUDE_PATH}/../recastnavigation/DetourTileCache/Include>
    $<BUILD_INTERFACE:${INCLUDE_PATH}/../recastnavigation/Recast/Include>
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    PRIVATE
    BabylonCpp
    json_hpp
)

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${BABYLON_UPPER}_STATIC_DEFINE>
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
if(OPTION_BUILD_TESTS)
    add_subdirectory(tests)
endif()

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
