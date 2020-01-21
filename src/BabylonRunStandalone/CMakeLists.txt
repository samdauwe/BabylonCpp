include(../../cmake/BuildEnvironment.cmake)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

set(TARGET BabylonRunStandalone)
file(GLOB sources *.*)
babylon_add_executable(${TARGET} ${sources})

target_link_libraries(${TARGET}
    PRIVATE
    BabylonImGui
    MaterialsLibrary
    Samples
    imgui
    imgui_utils
    json_hpp
)
