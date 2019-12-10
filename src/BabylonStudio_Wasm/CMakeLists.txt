if (EMSCRIPTEN)
    include(../../cmake/BuildEnvironment.cmake)

    set(TARGET BabylonStudio_Wasm)
    # project(${TARGET} C CXX)
    message(STATUS "App ${TARGET}")

    file(GLOB_RECURSE SOURCES_H_CPP
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.md
        )
    source_group_by_path_all(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_H_CPP})

    add_executable(${TARGET} ${SOURCES_H_CPP} imgui_sdl_emscripten_runner.cpp imgui_sdl_emscripten_runner.h)
    babylon_target_clang_tidy(${TARGET})

    target_link_libraries(${TARGET}
        PRIVATE
        imgui
        imgui_utils
        BabylonImGui
        BabylonCpp
        Samples
        glad
        json_hpp
    )

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${TARGET} PRIVATE --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_minimal.html)

endif(EMSCRIPTEN)
