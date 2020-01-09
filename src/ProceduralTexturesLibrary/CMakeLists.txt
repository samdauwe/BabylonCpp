# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET ProceduralTexturesLibrary)

# Print status message
message(STATUS "Lib ${TARGET}")

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #


# Meta information about the project
set(META_PROJECT_NAME        "ProceduralTexturesLibrary")
set(META_PROJECT_DESCRIPTION "")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Include, Source
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/${BABYLON_NAMESPACE}")
set(SOURCE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE BABYLON_PROCEDURALS_HEADERS ${INCLUDE_PATH}/*.h)
file(GLOB_RECURSE BABYLON_PROCEDURALS_SOURCES ${SOURCE_PATH}/proceduraltextureslibrary/*.cpp)
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${BABYLON_PROCEDURALS_HEADERS} ${BABYLON_PROCEDURALS_SOURCES})


# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
add_library(${TARGET}
    ${BABYLON_PROCEDURALS_SOURCES}
    ${BABYLON_PROCEDURALS_HEADERS}
)
babylon_target_clang_tidy(${TARGET})

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${TARGET} ALIAS ${TARGET})

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
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Libraries
target_link_libraries(${TARGET} PRIVATE BabylonCpp json_hpp)

# Compile definitions
target_compile_definitions(${TARGET}
    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${BABYLON_UPPER}_STATIC_DEFINE>
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
