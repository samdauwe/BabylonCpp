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
set(TARGET imgui_utils)

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
    )
set(ImGuiColorTextEditFolder ${CMAKE_CURRENT_SOURCE_DIR}/../../external/ImGuiColorTextEdit)
set(SOURCES_H_CPP 
    ${SOURCES_H_CPP} ${ImGuiColorTextEditFolder}/TextEditor.cpp
    ${SOURCES_H_CPP} ${ImGuiColorTextEditFolder}/TextEditor.h
    )
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_H_CPP})

# ============================================================================ #
#                       Create Library                                         #
# ============================================================================ #

# Build executable
add_library(${TARGET} STATIC ${SOURCES_H_CPP})
babylon_target_clang_tidy(${TARGET})

# Include directories
target_include_directories(${TARGET}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
    ${ImGuiColorTextEditFolder}/..
)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    glfw
    glad
    imgui
    ${OPENGL_LIBRARIES}
    BabylonCpp
)

if (UNIX AND NOT APPLE)
    target_link_libraries(${TARGET} PUBLIC X11)
endif()
