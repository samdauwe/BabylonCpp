# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Extensions)

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
#                       Create library                                         #
# ============================================================================ #

# Build library
babylon_add_library_glob(${TARGET})

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/recastnavigation/Detour/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/recastnavigation/DetourCrowd/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/recastnavigation/DetourTileCache/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/recastnavigation/Recast/Include>
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    PRIVATE
    BabylonCpp
    json_hpp
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
