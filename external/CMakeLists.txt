option(BUILD_OGL43 "Build OpenGL 4.3 examples" OFF)
mark_as_advanced(BUILD_OGL43)

find_package(OpenGL REQUIRED)

# Configure build environment
include(../cmake/BuildEnvironment.cmake)

# Earcut.hpp (A C++ port of earcut.js, a fast, header-only polygon triangulation library).
include_directories("earcut.hpp")
set(EARCUT_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/earcut.hpp/include/mapbox
    CACHE INTERNAL "Include directories of Earcut hpp")

# GLEW (The OpenGL Extension Wrangler Library).
SET(glew-cmake_BUILD_SHARED ON CACHE BOOL "Build the shared glew library" FORCE)
SET(glew-cmake_BUILD_STATIC OFF CACHE BOOL "Build the static glew library" FORCE)
SET(glew-cmake_BUILD_SINGLE_CONTEXT ON CACHE BOOL "Build the single context glew library" FORCE)
SET(glew-cmake_BUILD_MULTI_CONTEXT OFF CACHE BOOL "Build the multi context glew library" FORCE)
add_subdirectory(glew)
set(GLEW_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}/external/glew/include
    CACHE INTERNAL "Include directories of GLEW")
get_target_property(LIBRARY_OUTPUT_NAME
    libglew_shared GLEW_LIBRARY_OUTPUT_NAME
)

# GLFW (Free, open source, portable framework for OpenGL application development).
SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
add_subdirectory(glfw)
set(GLFW_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps
    CACHE INTERNAL "Include directories of GLFW")

# GoogleTest (Google's framework for writing C++ tests on a variety of platforms).
add_subdirectory(googletest)
set(GTEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include
    CACHE INTERNAL "Include directories of Google Test")

# imgui (Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies).
include_directories("imgui")
set(IMGUI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    CACHE INTERNAL "Include directories of imgui")
