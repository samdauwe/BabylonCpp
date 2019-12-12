# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

add_executable(imgui_example_glfw imgui_example_glfw.cpp ${imgui_dir}/examples/imgui_impl_glfw.cpp)
target_link_libraries(imgui_example_glfw PRIVATE glfw imgui)

if (BABYLON_SDL)
    add_executable(imgui_example_sdl imgui_example_sdl.cpp ${imgui_dir}/examples/imgui_impl_sdl.cpp)
    target_link_libraries(imgui_example_sdl PRIVATE SDL2 imgui)
endif()

file(GLOB sources_details details/*.*)
set(sources_demo
    imgui_runner.cpp
    imgui_runner.h
    imgui_runner_demo.cpp
    imgui_runner_demo.h
    ${sources_details}
    )

add_executable(imgui_runner_demo_glfw ${sources_demo})
add_executable(imgui_runner_demo_sdl ${sources_demo} ${imgui_dir}/examples/imgui_impl_sdl.cpp)
target_compile_definitions(imgui_runner_demo_glfw PRIVATE IMGUI_RUNNER_USE_GLFW)
target_compile_definitions(imgui_runner_demo_sdl PRIVATE IMGUI_RUNNER_USE_SDL)
target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_utils glfw)
target_link_libraries(imgui_runner_demo_sdl PRIVATE imgui_utils SDL2)
