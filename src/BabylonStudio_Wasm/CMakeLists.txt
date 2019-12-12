if (EMSCRIPTEN)
    option(BABYLON_BUILD_WASM "Build BabylonStudio_Wasm" ON)
else()
    option(BABYLON_BUILD_WASM "BabylonStudio_Wasm" OFF)
endif()

if (BABYLON_BUILD_WASM____HACKDISABLED)
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

    add_executable(${TARGET}
        ${SOURCES_H_CPP}
        /Users/pascal/dvp/OpenSource/BabylonCpp/external/imgui/examples/imgui_impl_sdl.cpp
#        /Users/pascal/dvp/OpenSource/BabylonCpp/external/imgui/examples/example_sdl_opengl3/main.cpp
        )
    babylon_target_clang_tidy(${TARGET})

    target_link_libraries(${TARGET}
        PRIVATE
        imgui
        imgui_utils
        BabylonImGui
        BabylonCpp
        Samples
        json_hpp
    )

    if (EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
        target_link_options(${TARGET} PRIVATE --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_minimal.html)
    else()

        if(APPLE)
#            find_package(OpenGLES CONFIG REQUIRED)
#            create_search_paths(/Developer/Platforms)
#            findpkg_framework(OpenGLES2)
#            set(OPENGLES2_LIBRARY "-framework OpenGLES")
        endif()

        # -DCMAKE_TOOLCHAIN_FILE=/Users/pascal/dvp/vcpkg/scripts/buildsystems/vcpkg.cmake
        find_package(SDL2 CONFIG REQUIRED)
        target_link_libraries(${TARGET} PRIVATE SDL2::SDL2main SDL2::SDL2-static)
    endif()

endif()
