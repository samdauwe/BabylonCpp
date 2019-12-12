# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

file(GLOB sources_details details/*.*)
set(sources_demo
    runner_babylon.cpp
    runner_babylon.h
    imgui_runner_demo2.cpp
    imgui_runner_demo2.h
    ${sources_details}
    )

add_executable(imgui_runner_demo_glfw ${sources_demo})
add_executable(imgui_runner_demo_sdl ${sources_demo} ${imgui_dir}/examples/imgui_impl_sdl.cpp)
target_compile_definitions(imgui_runner_demo_glfw PRIVATE IMGUI_RUNNER_USE_GLFW)
target_compile_definitions(imgui_runner_demo_sdl PRIVATE IMGUI_RUNNER_USE_SDL)
target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_utils glfw)
target_link_libraries(imgui_runner_demo_sdl PRIVATE imgui_utils SDL2)
