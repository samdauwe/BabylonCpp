# ============================================================================ #
#                       Library name and options                               #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Samples)

# Print status message
message(STATUS "Lib ${TARGET}")

# ============================================================================ #
#                       Project description and (meta) information             #
# ============================================================================ #

# Meta information about the project
set(META_PROJECT_NAME        "Samples")
set(META_PROJECT_DESCRIPTION "")

# Generate version-header
string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_version.h)


# ============================================================================ #
#                       Create library                                         #
# ============================================================================ #

# Build library
babylon_add_library_glob(${TARGET})

target_include_directories(${TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    imgui_utils
    BabylonCpp
    Extensions
    MaterialsLibrary
    ProceduralTexturesLibrary
    Loaders
    json_hpp
    subprocess_h
    magic_enum
)

if (WIN32)
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/ignore:4217 /ignore:4286 /ignore:4049")
endif()


# ============================================================================ #
#                       Setup test environment                                 #
# ============================================================================ #
add_subdirectory(tests)

# ============================================================================ #
#                       make samples info                                      #
# ============================================================================ #
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make_samples_info/make_samples_info.py
    COMMAND_ECHO STDOUT
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin
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
