# ============================================================================ #
#                                Project modules                               #
# ============================================================================ #

# CMake version
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# use ccache if present
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    # Support Unix Makefiles and Ninja
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

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
