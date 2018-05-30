# ============================================================================ #
#                                Project modules                               #
# ============================================================================ #

# CMake version
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# Generate folders for IDE targets (e.g., VisualStudio solutions)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(IDE_FOLDER "")

# Project name
project(BabylonCpp C CXX)

# Project modules
add_subdirectory(codegeneration)
add_subdirectory(external)
add_subdirectory(src)
