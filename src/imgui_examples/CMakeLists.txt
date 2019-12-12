set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/../../external/imgui)

add_executable(example_glfw example_glfw.cpp ${imgui_dir}/examples/imgui_impl_glfw.cpp)
target_link_libraries(example_glfw PRIVATE glfw imgui)

if (BABYLON_SDL)
    add_executable(example_sdl example_sdl.cpp ${imgui_dir}/examples/imgui_impl_sdl.cpp)
    target_link_libraries(example_sdl PRIVATE SDL2 imgui)
endif()
