# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

if (BABYLON_SDL_BACKEND)
    add_executable(imgui_runner_babylon_demo_sdl
        imgui_runner_babylon_demo_sdl.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_sdl PRIVATE imgui_runner_babylon)
endif()
if (BABYLON_GLFW_BACKEND)
    add_executable(imgui_runner_babylon_demo_glfw
        imgui_runner_babylon_demo_glfw.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_glfw PRIVATE imgui_runner_babylon)
endif()
if (BABYLON_GLFW_BACKEND)
    add_executable(imgui_runner_demo_glfw imgui_runner_demo_glfw.cpp)
    target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_runner_babylon)
endif()
