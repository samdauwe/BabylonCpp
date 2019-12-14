# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

file(GLOB sources *.*)
add_library(imgui_runner_babylon ${sources})
target_link_libraries(imgui_runner_babylon PUBLIC imgui_runner BabylonCpp)
target_include_directories(imgui_runner PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

if (IMGUI_RUNNER_USE_SDL)
    target_link_libraries(imgui_runner_babylon PUBLIC SDL2)
endif()
if (IMGUI_RUNNER_USE_GLFW)
    target_link_libraries(imgui_runner_babylon PUBLIC glfw)
endif()
