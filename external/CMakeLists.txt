option(BUILD_OGL43 "Build OpenGL 4.3 examples" OFF)
mark_as_advanced(BUILD_OGL43)

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()
include_directories(SYSTEM ${OPENGL_INCLUDE_DIRS})

# Configure build environment
include(../cmake/BuildEnvironment.cmake)

# Build gtest and gmock + macro "babylon_add_test"
# Adapted from https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
if(OPTION_BUILD_TESTS)
    mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
    )
    macro(babylon_add_test TESTNAME)
        message("babylon_add_test ${TESTNAME}")
        add_executable(${TESTNAME} ${ARGN})
        # Create namespaced alias
        add_executable(${META_PROJECT_NAME}::${TESTNAME} ALIAS ${TESTNAME})

        target_link_libraries(${TESTNAME} PRIVATE gtest gmock gtest_main)
        add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    endmacro()
    add_subdirectory(googletest)
endif(OPTION_BUILD_TESTS)


# Earcut.hpp (A C++ port of earcut.js, a fast, header-only polygon triangulation library).
include_directories(SYSTEM "earcut.hpp")
set(EARCUT_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/earcut.hpp/include/mapbox
    CACHE INTERNAL "Include directories of Earcut hpp")


# GLFW (Free, open source, portable framework for OpenGL application development).
if (IMGUI_RUNNER_USE_GLFW)
    SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
    SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
    SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_BUILD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
    add_subdirectory(glfw)
endif()

# json.hpp (JSON for Modern C++)
include_directories("json.hpp")
set(JSON_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
    CACHE INTERNAL "Include directories of json.hpp")
add_library(json_hpp INTERFACE)
target_include_directories(json_hpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp)

# glad as a standalone dynamic library
if (NOT EMSCRIPTEN)
    set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad CACHE INTERNAL "Directory of glad")
    set(GLAD_FILES
        ${GLAD_DIR}/src/glad.c
        ${GLAD_DIR}/include/glad/glad.h
        ${GLAD_DIR}/include/KHR/khrplatform.h)
    source_group_by_path_all(${GLAD_DIR} ${GLAD_FILES})
    add_library(glad ${GLAD_FILES})
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
else()
    add_library(glad INTERFACE)
    target_include_directories(glad INTERFACE ${CMAKE_CURRENT_LIST_DIR}/glad_wasm_dummy/include)
endif()

# imgui (Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies).
# imgui library as a standalone shared library
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui CACHE INTERNAL "imgui directory")
file(GLOB IMGUI_MAIN_FILES ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
if (NOT EMSCRIPTEN)
    file(GLOB IMGUI_BACKEND_FILES
        ${IMGUI_DIR}/examples/imgui_impl_glfw*.*
        ${IMGUI_DIR}/examples/imgui_impl_opengl*.*)
else()
    file(GLOB IMGUI_BACKEND_FILES
        ${IMGUI_DIR}/examples/imgui_impl_sdl*.*
        ${IMGUI_DIR}/examples/imgui_impl_opengl*.*)
endif()
set(IMGUI_FILES ${IMGUI_MAIN_FILES} ${IMGUI_BACKEND_FILES})
source_group_by_path_all(${IMGUI_DIR} ${IMGUI_FILES})
add_library(imgui ${IMGUI_FILES})
target_link_libraries(imgui PUBLIC glad)
if (NOT EMSCRIPTEN)
    target_link_libraries(imgui PUBLIC glfw)
#    find_package(SDL2 CONFIG REQUIRED)
#    target_link_libraries(imgui PUBLIC glad SDL2::SDL2main SDL2::SDL2-static)
    target_compile_definitions(imgui PUBLIC
        -DIMGUI_IMPL_OPENGL_LOADER_GLAD
        -DBABYLON_IMGUI__WITH_GLFW
#        -DBABYLON_IMGUI__WITH_SDL
        )
else()
    target_compile_definitions(imgui PUBLIC 
        -DBABYLON_IMGUI__WITH_SDL
    )
endif()

target_include_directories(imgui PUBLIC SYSTEM ${IMGUI_DIR} ${IMGUI_DIR}/examples)

if (WIN32)
  get_target_property(library_type imgui TYPE)
  if (library_type STREQUAL SHARED_LIBRARY)
    target_compile_definitions(imgui PRIVATE IMGUI_API=__declspec\(dllexport\))
    target_compile_definitions(imgui INTERFACE IMGUI_API=__declspec\(dllimport\))
  endif()
endif(WIN32)

# ImGuiColorTextEdit as a standalone object library
set(IMGUI_EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiColorTextEdit)
set(IMGUI_EDITOR_FILES
        ${IMGUI_EDITOR_DIR}/TextEditor.cpp
        ${IMGUI_EDITOR_DIR}/TextEditor.h)
source_group_by_path_all(${IMGUI_EDITOR_DIR} ${IMGUI_EDITOR_FILES})
add_library(ImGuiColorTextEdit OBJECT ${IMGUI_EDITOR_FILES})
target_include_directories(ImGuiColorTextEdit PUBLIC SYSTEM ${IMGUI_DIR} ${IMGUI_EDITOR_DIR} ${IMGUI_EDITOR_DIR}/..)
# Disable ImGuiColorTextEdit specific warnings
if (MSVC)
    target_compile_options(ImGuiColorTextEdit PRIVATE  /wd4244)
endif()
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    target_compile_options(ImGuiColorTextEdit PRIVATE -Wno-reorder -Wno-sign-compare -Wno-unused-variable)
endif()

# stb_image includes
set(STB_IMAGE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb_image/include
    CACHE INTERNAL "Include directories of stb_image")

# RuntimeCompiledCPlusPlus
if (NOT APPLE AND NOT EMSCRIPTEN)
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

# SDL
if (IMGUI_RUNNER_USE_SDL AND NOT EMSCRIPTEN) # SDL is already build by emscripten
    set (SdlDir ${CMAKE_CURRENT_SOURCE_DIR}/SDL)
    add_subdirectory(${SdlDir})
endif()
