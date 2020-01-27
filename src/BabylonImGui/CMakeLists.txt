# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET BabylonImGui)

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
#                       Create library                                         #
# ============================================================================ #

# Build library
babylon_add_library_glob(${TARGET})

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
    magic_enum
)

if (APPLE AND NOT EMSCRIPTEN) # Special case for Apple OpenGL framework
    target_link_libraries(${TARGET} PRIVATE "-framework OpenGL")
endif()

if (BABYLON_BUILD_PLAYGROUND)
    target_compile_definitions(${TARGET} PRIVATE -DBABYLON_BUILD_PLAYGROUND)
endif()

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
