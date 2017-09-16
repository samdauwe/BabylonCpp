# ============================================================================ #
#                            Executable name and options                       #
# ============================================================================ #

# Target name
set(TARGET BabylonCppBenchmarks_)
message(STATUS "Benchmarks ${TARGET}")

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
add_executable(${TARGET}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${TARGET} ALIAS ${TARGET})

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
    gmock-dev
)

# Compile definitions
target_compile_definitions(${TARGET}
    PRIVATE
)

# Compile options
target_compile_options(${TARGET}
    PRIVATE
)

# ============================================================================ #
#                            Run unit tests at build time                      #
# ============================================================================ #

# Check if unit tests should run at build time
get_target_property(TEST_EXCLUDE_FROM_DEFAULT_BUILD
    BabylonCppTests EXCLUDE_FROM_DEFAULT_BUILD
)

if(EXCLUDE_FROM_DEFAULT_BUILD)
    add_custom_command (
      TARGET ${TARGET} POST_BUILD
      COMMAND ${TARGET} --gtest_output=xml:${TARGET}.xml
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
    )
endif()
