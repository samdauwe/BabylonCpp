# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

file(GLOB sources *.*)
if (IMGUI_RUNNER_USE_SDL)
    set(sources ${sources} ${imgui_dir}/examples/imgui_impl_sdl.cpp)
endif()

add_library(imgui_runner ${sources})
target_link_libraries(imgui_runner PUBLIC imgui)
target_include_directories(imgui_runner PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

if (IMGUI_RUNNER_USE_GLFW)
    target_link_libraries(imgui_runner PUBLIC glfw)
    target_compile_definitions(imgui_runner PUBLIC IMGUI_RUNNER_USE_GLFW)
endif()

if (IMGUI_RUNNER_USE_SDL)
    target_link_libraries(imgui_runner PUBLIC SDL2)
    target_compile_definitions(imgui_runner PUBLIC IMGUI_RUNNER_USE_SDL)
endif()

if (IMGUI_IMPL_OPENGL_LOADER_GLAD)
    target_compile_definitions(imgui_runner PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()
