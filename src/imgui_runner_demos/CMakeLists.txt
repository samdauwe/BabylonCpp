# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(link_options --shell-file ${CMAKE_CURRENT_LIST_DIR}/../imgui_runner/runner_emscripten_shell.html)
endif()

if (IMGUI_SDL_BACKEND)
    add_executable(imgui_runner_babylon_demo_sdl
        imgui_runner_babylon_demo_sdl.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_sdl PRIVATE imgui_runner_babylon)
    if (EMSCRIPTEN)
        target_link_options(imgui_runner_babylon_demo_sdl PRIVATE ${link_options})
    endif()
endif()
if (IMGUI_GLFW_BACKEND)
    add_executable(imgui_runner_babylon_demo_glfw
        imgui_runner_babylon_demo_glfw.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_glfw PRIVATE imgui_runner_babylon)
endif()
if (IMGUI_GLFW_BACKEND)
    add_executable(imgui_runner_demo_glfw imgui_runner_demo_glfw.cpp)
    target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_runner_babylon)
endif()

