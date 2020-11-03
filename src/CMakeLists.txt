# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_IMGUI_BABYLON "Build the inspector debugging tool."  ON)


# ============================================================================ #
# Common variables used by all sub projects                                    #
# ============================================================================ #
include(../cmake/BuildEnvironment.cmake)
include(../cmake/BabylonAddTarget.cmake)

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


# Create feature detection header
# Compilers: https://cmake.org/cmake/help/v3.1/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
# Feature: https://cmake.org/cmake/help/v3.1/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html

# Check for availability of module; use pre-generated version if not found
write_compiler_detection_header(
    FILE ${FEATURE_FILE}
    PREFIX ${BABYLON_UPPER}
    COMPILERS AppleClang Clang GNU MSVC
    FEATURES cxx_alignas cxx_alignof cxx_constexpr cxx_final cxx_noexcept cxx_nullptr cxx_sizeof_member cxx_thread_local
    VERSION 3.2
)

# ============================================================================ #
#                            babylon_add_library                               #
# ============================================================================ #



# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #


#-- Libraries --##
add_subdirectory(BabylonCpp)
add_subdirectory(Extensions)
add_subdirectory(MaterialsLibrary)
add_subdirectory(ProceduralTexturesLibrary)

if (IOS)
    #-- Applications
    add_subdirectory(Standalone_iOS)
else()
    #-- Libraries --##
    add_subdirectory(imgui_utils)
    add_subdirectory(Samples)
    if (OPTION_BUILD_IMGUI_BABYLON)
        add_subdirectory(BabylonImGui)
    endif()

    #-- Applications
    add_subdirectory(BabylonStudio)
    add_subdirectory(BabylonRunStandalone)
    add_subdirectory(imgui_runner_demos)
    if (APPLE)
        add_subdirectory(Standalone_OSX)
    endif()
endif()
