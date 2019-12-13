# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

file(GLOB sources *.*)
if (BABYLON_SDL_BACKEND)
    set(sources ${sources} ${imgui_dir}/examples/imgui_impl_sdl.cpp)
endif()

add_library(imgui_runner ${sources})
target_link_libraries(imgui_runner PUBLIC imgui)
target_include_directories(imgui_runner PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

if (BABYLON_GLFW_BACKEND)
    target_link_libraries(imgui_runner PUBLIC glfw)
    target_compile_definitions(imgui_runner PUBLIC IMGUI_RUNNER_USE_GLFW)
endif()

if (BABYLON_SDL_BACKEND)
    target_link_libraries(imgui_runner PUBLIC SDL2)
    target_compile_definitions(imgui_runner PUBLIC IMGUI_RUNNER_USE_SDL)
endif()

