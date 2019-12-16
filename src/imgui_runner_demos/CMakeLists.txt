# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_link_options(--preload-file  ${CMAKE_SOURCE_DIR}/assets@/assets)
endif()

#
# imgui_runner_demos
#
if (IMGUI_RUNNER_USE_SDL)
    add_executable(imgui_runner_demo_sdl imgui_runner_demo_sdl.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_sdl PRIVATE imgui_utils)
endif()
if (IMGUI_RUNNER_USE_GLFW)
    add_executable(imgui_runner_demo_glfw imgui_runner_demo_glfw.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_glfw PRIVATE imgui_utils)
endif()
if (EMSCRIPTEN)
    add_executable(imgui_runner_demo_emscripten imgui_runner_demo_emscripten.cpp example_gui.cpp example_gui.h)
    target_link_libraries(imgui_runner_demo_emscripten PRIVATE imgui_utils)
endif()

#
# imgui_runner_babylon_demos
#
if (IMGUI_RUNNER_USE_SDL)
    add_executable(imgui_runner_babylon_demo_sdl
        imgui_runner_babylon_demo_sdl.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_sdl PRIVATE imgui_utils)
endif()
if (IMGUI_RUNNER_USE_GLFW)
    add_executable(imgui_runner_babylon_demo_glfw
        imgui_runner_babylon_demo_glfw.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_glfw PRIVATE imgui_utils)
endif()
if (EMSCRIPTEN)
    add_executable(imgui_runner_babylon_demo_emscripten
        imgui_runner_babylon_demo_emscripten.cpp
        runner_babylon_demo.cpp
        runner_babylon_demo.h)
    target_link_libraries(imgui_runner_babylon_demo_emscripten PRIVATE imgui_utils)
endif()


