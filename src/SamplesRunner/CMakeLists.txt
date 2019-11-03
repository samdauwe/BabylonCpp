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
set(TARGET SamplesRunner)

# Project name
project(${TARGET} C CXX)

# Print status message
message(STATUS "App ${TARGET}")


# ============================================================================ #
#                       Sources                                                #
# ============================================================================ #

# Source files
file(GLOB_RECURSE SOURCES_H_CPP
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.md
    )
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_H_CPP})

# ============================================================================ #
#                       Create executable                                      #
# ============================================================================ #

# Build executable
add_executable(${TARGET} ${SOURCES_H_CPP})

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${JSON_HPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_BINARY_DIR}/../BabylonCpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Extensions/include
    ${CMAKE_CURRENT_BINARY_DIR}/../Extensions/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Samples/include
)


# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    glfw
    glad
    imgui
    imgui_utils
    BabylonImGui
    ImGuiColorTextEdit
    ${OPENGL_LIBRARIES}
    BabylonCpp
    Samples
)

if (BABYLON_BUILD_PLAYGROUND)
    target_compile_definitions(${TARGET} PRIVATE -DBABYLON_BUILD_PLAYGROUND)
    target_link_libraries(${TARGET} PRIVATE RuntimeCompiler RuntimeObjectSystem)
endif()



# On Linux and other Unix platforms that use dlopen(), link with -ldl
if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
endif()

# Copy asset directories
## - Icons
copy_resource_dirs("${CMAKE_SOURCE_DIR}/assets/icons")
