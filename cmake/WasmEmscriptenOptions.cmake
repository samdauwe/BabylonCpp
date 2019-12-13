# Standard flags for emscripten
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-s USE_SDL=2 -s USE_WEBGL2=1 -s WASM=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")

# Debug build is  -O0 -g4 in order to have the call stack + source map
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g4 -s DISABLE_EXCEPTION_CATCHING=0")

    # source-map-base enables to be able to "kind of debug" from the js developper console
    # Advice: always debug the same exact source code on desktop before even trying to run in the browser
    add_link_options(-O0 --source-map-base http://localhost:8000/src/)
endif()

#add_link_options(--preload-file build/assets@/)

# Provide a default html shell
set(link_options_shell_emscripten --shell-file ${CMAKE_SOURCE_DIR}/src/imgui_runner/runner_emscripten_shell.html)
add_link_options(${link_options_shell_emscripten})
