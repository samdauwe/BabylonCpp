# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_INSPECTOR "Build the inspector debugging tool."  ON)
option(OPTION_BUILD_LOADERS   "Build the loader plugins."            ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(OPTION_BUILD_INSPECTOR OFF)
    set(OPTION_BUILD_LOADERS   OFF)
endif()

# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #

#-- Libraries --##
set(IDE_FOLDER "")
# - Babylon.js Cpp
add_subdirectory(BabylonCpp)
# - Extensions
add_subdirectory(Extensions)
# - Materials Library
add_subdirectory(MaterialsLibrary)
# - Procedural Textures Library
add_subdirectory(ProceduralTexturesLibrary)
if (OPTION_BUILD_LOADERS)
    # - Loaders
    add_subdirectory(Loaders)
endif()
# - Inspector
if (OPTION_BUILD_INSPECTOR)
    # Include Inspector
    add_subdirectory(Inspector)
endif()
# - Samples
add_subdirectory(Samples)

#-- Applications --##
# - Sample Launcher
add_subdirectory(SampleLauncher)
# - Studio
# add_subdirectory(Studio)
