# ============================================================================ #
#                            Executable name and options                       #
# ============================================================================ #

# Target name
set(TARGET ExtensionsTests)
message(STATUS "Test ${TARGET}")

# ============================================================================ #
#                            Sources                                           #
# ============================================================================ #

# Sources
file(GLOB_RECURSE SRC_FILES *.cpp)
set(sources
    ${SRC_FILES}
)

# ============================================================================ #
#                            Create executable                                 #
# ============================================================================ #

# Build executable
package_add_test(${TARGET} ${sources})
babylon_target_clang_tidy(${TARGET})

# Project options
set_target_properties(${TARGET}
    PROPERTIES ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)

# Include directories
target_include_directories(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

# Libraries
target_link_libraries(${TARGET}
    PRIVATE
    BabylonCpp
    Extensions
)

# Compile definitions
target_compile_definitions(${TARGET}
    PRIVATE
)

# Compile options
target_compile_options(${TARGET}
    PRIVATE
)
