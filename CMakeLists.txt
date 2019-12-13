# ============================================================================ #
#                                Project modules                               #
# ============================================================================ #

# CMake version
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

option(BABYLON_SDL_BACKEND "Build babylon sdl version" ON)
option(BABYLON_GLFW_BACKEND "Build babylon glfw version" ON)

# Compile to Wasm
option(BABYLON_WASM "Target emscriptem / wasm" OFF)
if (BABYLON_WASM)
    # emscripten is detected via
    #  - if (EMSCRIPTEN) in cmake code
    #  - #ifdef __EMSCRIPTEN__ in c++ code
    set(EMSCRIPTEN ON)
    set(BUILD_SHARED_LIBS OFF)
    set(BABYLON_DISABLE_INSTALL ON)
    set(OPTION_BUILD_TESTS_DEFAULT OFF)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/WasmEmscriptenOptions.cmake)
else()
    set(OPTION_BUILD_TESTS_DEFAULT ON)
endif()

if (NOT BABYLON_WASM)
    option(BUILD_SHARED_LIBS     "Build shared instead of static libraries."     ON)
endif()

# use ccache if present
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    option(USE_CCACHE "Use ccache for the build" OFF)
    if (USE_CCACHE)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
endif()

option(OPTION_BUILD_TESTS    "Build tests."       ${OPTION_BUILD_TESTS_DEFAULT})
if(OPTION_BUILD_TESTS)
    enable_testing()
endif()

# use clang-tidy if present
option(BABYLON_USE_CLANG_TIDY "Use clang-tidy static analysis" OFF)
if (BABYLON_USE_CLANG_TIDY)
  find_program(
    CLANG_TIDY_EXE
    HINTS ~/bin/clang/bin/
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
    )
  if(NOT CLANG_TIDY_EXE)
    message(FATAL "clang-tidy not found.")
  else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    # set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-alpha.*")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
  endif()
endif()

function(babylon_target_clang_tidy target)
  if (BABYLON_USE_CLANG_TIDY)
    set_target_properties(
      ${target} PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
  endif()
endfunction()

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project name
project(BabylonCpp C CXX)

# Project modules
add_subdirectory(codegeneration)
add_subdirectory(external)
add_subdirectory(src)
