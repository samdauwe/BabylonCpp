# ============================================================================ #
#                                Project modules                               #
# ============================================================================ #

# CMake version
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# use ccache if present
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    option(USE_CCACHE "Use ccache for the build" OFF)
    if (USE_CCACHE)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
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
set(IDE_FOLDER "")

# Project name
project(BabylonCpp C CXX)

if (MSVC)
    enable_testing()
endif()

# Project modules
add_subdirectory(codegeneration)
add_subdirectory(external)
add_subdirectory(src)
