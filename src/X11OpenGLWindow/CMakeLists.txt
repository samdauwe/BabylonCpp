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
set(TARGET X11OpenGLWindow)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Sources
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(BABYLON_X11_OPENGL_WINDOW_SOURCES
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# ============================================================================ #
#                       Create executable                                      #
# ============================================================================ #

# Build executable
add_executable(${TARGET}
    ${BABYLON_X11_OPENGL_WINDOW_SOURCES}
)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${JSON_HPP_INCLUDE_DIRS}
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
    ${OPENGL_LIBRARIES}
    BabylonCpp
    Samples
)

# On Linux and other Unix platforms that use dlopen(), link with -ldl
if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lX11")
endif()
