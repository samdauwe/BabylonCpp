# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(link_options_emscripten --shell-file ${CMAKE_CURRENT_LIST_DIR}/../imgui_runner/runner_emscripten_shell.html)
endif()

#
# imgui_runner_demos
#
if (IMGUI_SDL_BACKEND)
    add_executable(imgui_runner_demo_sdl imgui_runner_demo_sdl.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_sdl PRIVATE imgui_runner_babylon)
endif()
if (IMGUI_GLFW_BACKEND)
    add_executable(imgui_runner_demo_glfw imgui_runner_demo_glfw.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_runner_babylon)
endif()
if (EMSCRIPTEN)
    add_executable(imgui_runner_demo_emscripten imgui_runner_demo_emscripten.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_emscripten PRIVATE imgui_runner_babylon)

    target_link_options(imgui_runner_demo_emscripten PRIVATE ${link_options_emscripten})
endif()

#
# imgui_runner_babylon_demos
#
if (IMGUI_SDL_BACKEND)
    add_executable(imgui_runner_babylon_demo_sdl
        imgui_runner_babylon_demo_sdl.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_sdl PRIVATE imgui_runner_babylon)
endif()
if (IMGUI_GLFW_BACKEND)
    add_executable(imgui_runner_babylon_demo_glfw
        imgui_runner_babylon_demo_glfw.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_glfw PRIVATE imgui_runner_babylon)
endif()
if (EMSCRIPTEN)
    add_executable(imgui_runner_babylon_demo_emscripten
        imgui_runner_babylon_demo_emscripten.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_emscripten PRIVATE imgui_runner_babylon)

    target_link_options(imgui_runner_babylon_demo_emscripten PRIVATE ${link_options_emscripten})
endif()


