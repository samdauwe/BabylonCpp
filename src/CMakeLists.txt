# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_IMGUI_BABYLON "Build the inspector debugging tool."  ON)


# ============================================================================ #
# Common variables used by all sub projects                                    #
# ============================================================================ #
include(../cmake/BuildEnvironment.cmake)

set(BABYLON_NAMESPACE babylon)
string(TOUPPER ${BABYLON_NAMESPACE} BABYLON_UPPER)
set(FEATURE_FILE "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_features.h")
set(EXPORT_FILE  "include/${BABYLON_NAMESPACE}/${BABYLON_NAMESPACE}_api.h")
set(EXPORT_MACRO "${BABYLON_UPPER}_SHARED_EXPORT")

# Get git revision
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING "${GIT_SHA1}" 0 12 GIT_REV)

# Meta information about the projects
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN       "")
set(META_AUTHOR_MAINTAINER   "")
set(META_VERSION_MAJOR       "4")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION_REVISION    "${GIT_REV}")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")
# You need set META_PROJECT_NAME and META_PROJECT_DESCRIPTION project by project
#set(META_PROJECT_NAME        "")
#set(META_PROJECT_DESCRIPTION "")


# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #


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
