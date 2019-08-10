# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Target name
set(TARGET SampleLauncher)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")

# imgui_babylon and Inspector
if (OPTION_BUILD_IMGUI_BABYLON)
    # Set define for usage in source code
    add_definitions(-DWITH_IMGUI_BABYLON)
endif()

# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Header files
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# Source files
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Sources
set(BABYLON_SAMPLES_SOURCES
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# ============================================================================ #
#                       Create executable                                      #
# ============================================================================ #

# Build executable
add_executable(${TARGET}
    ${BABYLON_SAMPLES_SOURCES}
)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${JSON_HPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extensions/include
    ${CMAKE_CURRENT_BINARY_DIR}/../Extensions/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Samples/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (OPTION_BUILD_IMGUI_BABYLON)
    target_include_directories(${TARGET}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../imgui_babylon/include
        ${CMAKE_CURRENT_BINARY_DIR}/../imgui_babylon/include
    )
endif()

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    glfw
    ${OPENGL_LIBRARIES}
    BabylonCpp
    Samples
    glad
)

if (OPTION_BUILD_IMGUI_BABYLON)
    target_link_libraries(${TARGET}
        PUBLIC
        imgui_babylon
    )
endif()

# On Linux and other Unix platforms that use dlopen(), link with -ldl
if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
endif()

# Copy asset directories
## - Icons
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/icons")
