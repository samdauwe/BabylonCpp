# ============================================================================ #
#                       Application name and options                           #
# ============================================================================ #

# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

if (EMSCRIPTEN)
    set(repo_assets_folder ${CMAKE_SOURCE_DIR}/assets)
    set(emscripten_static_assets_folder ${CMAKE_BINARY_DIR}/build/bin/emscripten_static_assets)
    set(emscripten_http_assets_folder ${CMAKE_BINARY_DIR}/build/bin/emscripten_http_assets)

    function(copy_static_assets)
        file(COPY ${repo_assets_folder}/fonts DESTINATION ${emscripten_static_assets_folder})
        file(COPY ${CMAKE_SOURCE_DIR}/src/Samples DESTINATION ${emscripten_static_assets_folder})
    endfunction()

    function(copy_http_assets)
        file(COPY ${repo_assets_folder} DESTINATION ${emscripten_http_assets_folder})
    endfunction()

    copy_static_assets()
    copy_http_assets()

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(
        ${EMSCRIPTEN_LINK_OPTIONS}
        --preload-file ${emscripten_static_assets_folder}@/emscripten_static_assets_folder
    )
endif()

# OpenGL dependency
set(OpenGL_GL_PREFERENCE GLVND)
if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
endif()
include_directories(${OPENGL_INCLUDE_DIRS})

# Target name
set(TARGET BabylonStudio)

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

# ============================================================================ #
#                       Create executable                                      #
# ============================================================================ #

# Build executable
babylon_add_executable(${TARGET} ${SOURCES_H_CPP})

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    imgui
    imgui_utils
    BabylonImGui
    BabylonCpp
    Samples
    json_hpp
    subprocess_h
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
