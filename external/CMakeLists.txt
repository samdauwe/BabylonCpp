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


# imgui + backends (SDL, GLFW) compilation
include("CMakeLists_imgui.cmake")


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


# json.hpp (JSON for Modern C++)
include_directories("json.hpp")
set(JSON_HPP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
    CACHE INTERNAL "Include directories of json.hpp")
add_library(json_hpp INTERFACE)
target_include_directories(json_hpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp)


# glad as a standalone library
# (in the case of emscripten, fake library that includes GlES)
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


# stb_image
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

# promise-cpp
add_library(promise_cpp INTERFACE)
target_include_directories(promise_cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/promise-cpp)
set(PROMISE_CPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/promise-cpp
    CACHE INTERNAL "Include directories of promise-cpp")
