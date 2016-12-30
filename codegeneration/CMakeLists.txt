# CMake version
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(scripts)

find_package(PythonInterp 2.7.0 REQUIRED)

# Sources
file(GLOB_RECURSE SCRIPT_FILES *.py)
add_custom_target(scripts SOURCES ${SCRIPT_FILES})
