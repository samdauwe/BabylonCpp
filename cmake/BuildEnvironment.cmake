# ============================================================================ #
#                                CMake options                                 #
# ============================================================================ #

# CMake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Be nice to vim users
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(WriterCompilerDetectionHeaderFound NOTFOUND)

include(GenerateExportHeader)
include(ExternalProject)

# This module is only available with CMake >=3.1, so check whether it could be found
include(WriteCompilerDetectionHeader OPTIONAL RESULT_VARIABLE WriterCompilerDetectionHeaderFound)
# BUT in CMake 3.1 this module doesn't recognize AppleClang as compiler, so disable it
if (${CMAKE_VERSION} VERSION_LESS "3.2")
    set(WriterCompilerDetectionHeaderFound NOTFOUND)
endif()

# Git version
include(${CMAKE_MODULE_PATH}/GetGitRevisionDescription.cmake)

# Custom cmake functions
include(${CMAKE_MODULE_PATH}/Custom.cmake)

# Compiler settings and options
include(${CMAKE_MODULE_PATH}/CompileOptions.cmake)

# Set policies
set_policy(CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.

# ============================================================================ #
#   Set output paths for binaries and libraries if not provided by the user    #
# ============================================================================ #

# Prohibit in-source builds
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please use "
                        "./configure to choose a build directory and "
                        "initialize the build configuration.")
endif()

# Set archive output path if not defined by user
if("${ARCHIVE_OUTPUT_PATH}" STREQUAL "")
  set(ARCHIVE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/build/lib")
endif()

# Set binary output path if not defined by user
if("${EXECUTABLE_OUTPUT_PATH}" STREQUAL "")
  set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/build/bin")
endif()

# Set library output path if not defined by user
if("${LIBRARY_OUTPUT_PATH}" STREQUAL "")
  set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/build/lib")
endif()

# Set assets output path if not defined by user
if("${ASSETS_OUTPUT_PATH}" STREQUAL "")
  set(ASSETS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/build/assets")
endif()

# Set archive output path if not defined by user
if("${INCLUDE_OUTPUT_PATH}" STREQUAL "")
  set(INCLUDE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/include")
endif()

# Set cmake output path if not defined by user
if("${CMAKE_OUTPUT_PATH}" STREQUAL "")
  set(CMAKE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/cmake")
endif()

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL
