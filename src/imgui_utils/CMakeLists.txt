# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()
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
source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_H_CPP})

# ============================================================================ #
#                       Create Library                                         #
# ============================================================================ #

# Build executable
add_library(${TARGET} STATIC ${SOURCES_H_CPP} $<TARGET_OBJECTS:ImGuiColorTextEdit>)
babylon_target_clang_tidy(${TARGET})

# Include directories
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
# include ImGuiColorTextEdit
target_include_directories(${TARGET} PRIVATE SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../../external)

# Libraries
target_link_libraries(${TARGET}
    PUBLIC
    imgui
    BabylonCpp
)
if (NOT EMSCRIPTEN)
    target_link_libraries(${TARGET} PUBLIC ${OPENGL_LIBRARIES})
endif()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    target_link_libraries(${TARGET} PUBLIC X11)
endif()
