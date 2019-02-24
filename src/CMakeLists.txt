# ============================================================================ #
#                            Configuration options                             #
# ============================================================================ #
option(OPTION_BUILD_INSPECTOR "Build shared the inspector debugging tool."  ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(OPTION_BUILD_INSPECTOR OFF)
endif()

# ============================================================================ #
#                            Sub-projects                                      #
# ============================================================================ #

#### Libraries ####
set(IDE_FOLDER "")
# - Oimo CPP
add_subdirectory(OimoCpp)
# - Babylon.js Cpp
add_subdirectory(BabylonCpp)
# - Extensions
add_subdirectory(Extensions)
# - Materials Library
add_subdirectory(MaterialsLibrary)
# - Procedural Textures Library
add_subdirectory(ProceduralTexturesLibrary)
# - Loaders
add_subdirectory(Loaders)
# - Inspector
if (OPTION_BUILD_INSPECTOR)
    # Include Inspector
    add_subdirectory(Inspector)
endif()
# - Samples
add_subdirectory(Samples)

#### Applications ####
# - Sample Launcher
add_subdirectory(SampleLauncher)
