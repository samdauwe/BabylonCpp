# imgui + backends (SDL, GLFW) compilation

option(IMGUI_RUNNER_USE_SDL "Build babylon sdl version" ON)
set(IMGUI_RUNNER_USE_SDL OFF) # No SDL support by default
if (NOT EMSCRIPTEN)
    option(IMGUI_RUNNER_USE_GLFW "Build babylon glfw version" ON)
else()
    set(IMGUI_RUNNER_USE_GLFW OFF)
endif()


# GLFW (Free, open source, portable framework for OpenGL application development).
if (IMGUI_RUNNER_USE_GLFW)
    SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
    SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
    SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_BUILD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
    add_subdirectory(glfw)
endif()

# SDL
if (IMGUI_RUNNER_USE_SDL AND NOT EMSCRIPTEN) # SDL is already build by emscripten
    set (SdlDir ${CMAKE_CURRENT_SOURCE_DIR}/SDL)
    add_subdirectory(${SdlDir})
endif()


# imgui (Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies).
# Sources
set(imgui_dir ${CMAKE_CURRENT_SOURCE_DIR}/imgui CACHE INTERNAL "imgui directory")
file(GLOB imgui_sources ${imgui_dir}/*.h ${imgui_dir}/*.cpp)
set(imgui_sources ${imgui_sources}
  ${imgui_dir}/backends/imgui_impl_opengl3.h
  ${imgui_dir}/backends/imgui_impl_opengl3.cpp
)
if (IMGUI_RUNNER_USE_SDL)
    set(imgui_sources ${imgui_sources}
        ${imgui_dir}/backends/imgui_impl_sdl.h
        ${imgui_dir}/backends/imgui_impl_sdl.cpp
     )
endif()
if (IMGUI_RUNNER_USE_GLFW)
    set(imgui_sources ${imgui_sources}
        ${imgui_dir}/backends/imgui_impl_glfw.h
        ${imgui_dir}/backends/imgui_impl_glfw.cpp
     )
endif()
# Add library
source_group_by_path_all(${imgui_dir} ${imgui_sources})
add_library(imgui ${imgui_sources})
# Include dirs
target_include_directories(imgui PUBLIC SYSTEM ${imgui_dir} ${imgui_dir}/backends)
# Link
target_link_libraries(imgui PUBLIC glad)
if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    target_link_libraries(imgui PUBLIC X11)
endif()
if (NOT EMSCRIPTEN)
    if (IMGUI_RUNNER_USE_GLFW)
        target_link_libraries(imgui PUBLIC glfw)
    endif()
    if (IMGUI_RUNNER_USE_SDL)
        target_link_libraries(imgui PUBLIC SDL2)
    endif()
endif()
# Compile definitions
if (IMGUI_RUNNER_USE_GLFW)
    target_compile_definitions(imgui PUBLIC IMGUI_RUNNER_USE_GLFW)
endif()
if (IMGUI_RUNNER_USE_SDL)
    target_compile_definitions(imgui PUBLIC IMGUI_RUNNER_USE_SDL SDL_MAIN_HANDLED)
endif()
if (NOT EMSCRIPTEN)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()
# DLL / Dynamic library options
if (WIN32)
  get_target_property(library_type imgui TYPE)
  if (library_type STREQUAL SHARED_LIBRARY)
    target_compile_definitions(imgui PRIVATE IMGUI_API=__declspec\(dllexport\))
    target_compile_definitions(imgui INTERFACE IMGUI_API=__declspec\(dllimport\))
  endif()
endif(WIN32)


# ImGuiColorTextEdit as a standalone object library
set(IMGUI_EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiColorTextEdit)
set(IMGUI_EDITOR_FILES
        ${IMGUI_EDITOR_DIR}/TextEditor.cpp
        ${IMGUI_EDITOR_DIR}/TextEditor.h)
source_group_by_path_all(${IMGUI_EDITOR_DIR} ${IMGUI_EDITOR_FILES})
add_library(ImGuiColorTextEdit OBJECT ${IMGUI_EDITOR_FILES})
target_include_directories(ImGuiColorTextEdit PUBLIC SYSTEM ${imgui_dir} ${IMGUI_EDITOR_DIR} ${IMGUI_EDITOR_DIR}/..)
# Disable ImGuiColorTextEdit specific warnings
if (MSVC)
    target_compile_options(ImGuiColorTextEdit PRIVATE  /wd4244)
endif()
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    target_compile_options(ImGuiColorTextEdit PRIVATE -Wno-reorder -Wno-sign-compare -Wno-unused-variable)
endif()

