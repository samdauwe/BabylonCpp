include(../../cmake/BuildEnvironment.cmake)
set(TARGET BabylonRunStandalone)
file(GLOB sources *.*)
add_executable(${TARGET} ${sources})
target_link_libraries(${TARGET}
    PRIVATE
    BabylonImGui
    MaterialsLibrary
    Samples
    imgui
    imgui_utils
    imgui_runner_babylon
    json_hpp
    )
