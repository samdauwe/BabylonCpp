# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

file(GLOB sources *.*)
add_library(imgui_runner_babylon ${sources})
target_link_libraries(imgui_runner_babylon PUBLIC imgui_runner BabylonCpp)

if (IMGUI_SDL_BACKEND)
    target_link_libraries(imgui_runner_babylon PUBLIC SDL2)
endif()
if (IMGUI_GLFW_BACKEND)
    target_link_libraries(imgui_runner_babylon PUBLIC glfw)
endif()
