# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_IMGUI_BABYLON "Build the inspector debugging tool."  ON)
option(OPTION_BUILD_LOADERS       "Build the loader plugins."            ON)

# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #

#-- Libraries --##
add_subdirectory(imgui_runner)
add_subdirectory(imgui_runner_babylon)
add_subdirectory(imgui_utils)
add_subdirectory(BabylonCpp)
add_subdirectory(Extensions)
add_subdirectory(MaterialsLibrary)
add_subdirectory(ProceduralTexturesLibrary)
add_subdirectory(Samples)
if (OPTION_BUILD_LOADERS)
    add_subdirectory(Loaders)
endif()
if (OPTION_BUILD_IMGUI_BABYLON)
    add_subdirectory(BabylonImGui)
endif()

#-- Applications
add_subdirectory(BabylonStudio)
add_subdirectory(BabylonStudio_Wasm)
add_subdirectory(imgui_runner_demos)
