# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# OpenGL dependency
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLEW dependency
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Target name
set(TARGET SampleLauncher)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")


# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Sources
file(GLOB_RECURSE HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
                                ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(BABYLON_SAMPLES_SOURCES
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# ============================================================================ #
#                       Create executable                                      #
# ============================================================================ #

# Build executable
add_executable(${TARGET}
    ${BABYLON_SAMPLES_SOURCES}
)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extensions/include
    ${CMAKE_CURRENT_BINARY_DIR}/../Extensions/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Samples/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    glfw
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    BabylonCpp
    Samples
)