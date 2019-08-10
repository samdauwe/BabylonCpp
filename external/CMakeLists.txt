option(BUILD_OGL43 "Build OpenGL 4.3 examples" OFF)
mark_as_advanced(BUILD_OGL43)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Configure build environment
include(../cmake/BuildEnvironment.cmake)

# Earcut.hpp (A C++ port of earcut.js, a fast, header-only polygon triangulation library).
include_directories("earcut.hpp")
set(EARCUT_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/earcut.hpp/include/mapbox
    CACHE INTERNAL "Include directories of Earcut hpp")


# GLFW (Free, open source, portable framework for OpenGL application development).
SET(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
add_subdirectory(glfw)

# GoogleTest (Google's framework for writing C++ tests on a variety of platforms).
add_subdirectory(googletest)
set(GTEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include
    CACHE INTERNAL "Include directories of Google Test")


# json.hpp (JSON for Modern C++)
include_directories("json.hpp")
set(JSON_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
    CACHE INTERNAL "Include directories of json.hpp")

# imgui (Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies).
# imgui library as a standalone *static* library (shared libraries are a portability nightmare)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui CACHE INTERNAL "imgui directory")
file(GLOB IMGUI_MAIN_FILES ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
file(GLOB IMGUI_BACKEND_FILES 
    ${IMGUI_DIR}/examples/imgui_impl_glfw*.* 
    ${IMGUI_DIR}/examples/imgui_impl_opengl*.*)
set(IMGUI_FILES ${IMGUI_MAIN_FILES} ${IMGUI_BACKEND_FILES})
source_group_by_path_all(${IMGUI_DIR} ${IMGUI_FILES})
add_library(imgui STATIC ${IMGUI_FILES})
target_link_libraries(imgui PRIVATE glfw glad)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/examples)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)

# glad as a standalone *static* library
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad CACHE INTERNAL "Directory of glad")
set(GLAD_FILES 
    ${GLAD_DIR}/src/glad.c
    ${GLAD_DIR}/include/glad/glad.h
    ${GLAD_DIR}/include/KHR/khrplatform.h)
source_group_by_path_all(${GLAD_DIR} ${GLAD_FILES})
add_library(glad STATIC ${GLAD_FILES})
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)
