# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Target name
set(TARGET SampleLauncher)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")

# Inspector
if (OPTION_BUILD_INSPECTOR)
    # Set define for usage in source code
    add_definitions(-DWITH_INSPECTOR)
endif()

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Sources
file(GLOB_RECURSE HEADER_FILES
    ${GLXW_INCLUDE_DIRS}/GL/*.h
    ${GLXW_INCLUDE_DIRS}/GLXW/glxw.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

file(GLOB_RECURSE SOURCE_FILES
    ${GLXW_SOURCE_DIRS}/glxw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
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
    ${GLXW_INCLUDE_DIRS}
    ${JSON_HPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extensions/include
    ${CMAKE_CURRENT_BINARY_DIR}/../Extensions/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Samples/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (OPTION_BUILD_INSPECTOR)
    target_include_directories(${TARGET}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Inspector/include
        ${CMAKE_CURRENT_BINARY_DIR}/../Inspector/include
    )
endif()

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    glfw
    ${OPENGL_LIBRARIES}
    BabylonCpp
    Samples
)

if (OPTION_BUILD_INSPECTOR)
    target_link_libraries(${TARGET}
        PUBLIC
        Inspector
    )
endif()

# On Linux and other Unix platforms that use dlopen(), link with -ldl
if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
endif()

# Copy asset directories
## - Icons
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/icons")
