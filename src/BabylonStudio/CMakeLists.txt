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
set(TARGET BabylonStudio)

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
babylon_target_clang_tidy(${TARGET})

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    imgui
    imgui_utils
    BabylonImGui
    BabylonCpp
    Samples
    json_hpp
)
if (NOT EMSCRIPTEN)
    target_link_libraries(${TARGET} PRIVATE ${OPENGL_LIBRARIES})
endif()

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
