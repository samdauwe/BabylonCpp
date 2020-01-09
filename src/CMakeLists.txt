# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_IMGUI_BABYLON "Build the inspector debugging tool."  ON)

# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #

# Common variables used by all sub libraries
set(BABYLON_NAMESPACE babylon)
string(TOUPPER ${BABYLON_NAMESPACE} BABYLON_UPPER)
set(FEATURE_FILE "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_features.h")
set(EXPORT_FILE  "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_api.h")
set(EXPORT_MACRO "${BABYLON_UPPER}_SHARED_EXPORT")


#-- Libraries --##
add_subdirectory(imgui_utils)
add_subdirectory(BabylonCpp)
add_subdirectory(Extensions)
add_subdirectory(MaterialsLibrary)
add_subdirectory(ProceduralTexturesLibrary)
add_subdirectory(Samples)
add_subdirectory(Loaders)
if (OPTION_BUILD_IMGUI_BABYLON)
    add_subdirectory(BabylonImGui)
endif()

#-- Applications
add_subdirectory(BabylonStudio)
add_subdirectory(BabylonRunStandalone)
add_subdirectory(imgui_runner_demos)
