option(BUILD_OGL43 "Build OpenGL 4.3 examples" OFF)
mark_as_advanced(BUILD_OGL43)

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(SYSTEM ${OPENGL_INCLUDE_DIRS})

# Configure build environment
include(../cmake/BuildEnvironment.cmake)

# Earcut.hpp (A C++ port of earcut.js, a fast, header-only polygon triangulation library).
include_directories(SYSTEM "earcut.hpp")
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

# json.hpp (JSON for Modern C++)
include_directories("json.hpp")
set(JSON_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
    CACHE INTERNAL "Include directories of json.hpp")

# glad as a standalone dynamic library
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad CACHE INTERNAL "Directory of glad")
set(GLAD_FILES
    ${GLAD_DIR}/src/glad.c
    ${GLAD_DIR}/include/glad/glad.h
    ${GLAD_DIR}/include/KHR/khrplatform.h)
source_group_by_path_all(${GLAD_DIR} ${GLAD_FILES})
add_library(glad SHARED ${GLAD_FILES})
target_include_directories(glad PUBLIC SYSTEM ${GLAD_DIR}/include)
if(WIN32)
    target_link_libraries(glad PUBLIC opengl32.lib)
else()
    target_link_libraries(glad PUBLIC ${OPENGL_LIBRARIES})
endif()
get_target_property(library_type glad TYPE)
if (library_type STREQUAL SHARED_LIBRARY)
  # If glad is a shared lobrary, define the macro GLAD_API_EXPORT on the command line.
  target_compile_definitions(glad PRIVATE GLAD_GLAPI_EXPORT)
  target_compile_definitions(glad PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD)
endif()

# imgui (Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies).
# imgui library as a standalone shared library
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui CACHE INTERNAL "imgui directory")
file(GLOB IMGUI_MAIN_FILES ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
file(GLOB IMGUI_BACKEND_FILES
    ${IMGUI_DIR}/examples/imgui_impl_glfw*.*
    ${IMGUI_DIR}/examples/imgui_impl_opengl*.*)
set(IMGUI_FILES ${IMGUI_MAIN_FILES} ${IMGUI_BACKEND_FILES})
source_group_by_path_all(${IMGUI_DIR} ${IMGUI_FILES})
add_library(imgui SHARED ${IMGUI_FILES})
target_link_libraries(imgui PRIVATE glfw glad)
target_include_directories(imgui PUBLIC SYSTEM ${IMGUI_DIR} ${IMGUI_DIR}/examples)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
if (WIN32)
  get_target_property(library_type imgui TYPE)
  if (library_type STREQUAL SHARED_LIBRARY)
    target_compile_definitions(imgui PRIVATE IMGUI_API=__declspec\(dllexport\))
    target_compile_definitions(imgui INTERFACE IMGUI_API=__declspec\(dllimport\))
  endif()
endif(WIN32)

# RuntimeCompiledCPlusPlus
if (NOT APPLE)
    option(BABYLON_BUILD_PLAYGROUND "Build the interactive playground (modify and run the playground code interactively without exiting)" ON)
else()
    option(BABYLON_BUILD_PLAYGROUND "Build the interactive playground (modify and run the playground code interactively without exiting)" OFF)
endif()
if (BABYLON_BUILD_PLAYGROUND)
    set (RuntimeCompiledCPlusPlus_Dir ${CMAKE_CURRENT_SOURCE_DIR}/RuntimeCompiledCPlusPlus/Aurora)
    SET(BUILD_EXAMPLES OFF CACHE BOOL "Build RuntimeCompiledCPlusPlus examples" FORCE)
    add_subdirectory(${RuntimeCompiledCPlusPlus_Dir})
    target_include_directories(RuntimeCompiler INTERFACE SYSTEM ${RuntimeCompiledCPlusPlus_Dir})
    target_include_directories(RuntimeObjectSystem INTERFACE SYSTEM ${RuntimeCompiledCPlusPlus_Dir})
endif()
