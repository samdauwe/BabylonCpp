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

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Meta information about the project
set(META_PROJECT_NAME        "imgui_babylon")
set(META_PROJECT_DESCRIPTION "Inspector and imgui scene widget for BabylonCpp")

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

# Create API export header
generate_export_header(${TARGET}
    EXPORT_FILE_NAME  ${EXPORT_FILE}
    EXPORT_MACRO_NAME ${EXPORT_MACRO}
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
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${BABYLON_UPPER}_STATIC_DEFINE>
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
