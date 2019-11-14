# ============================================================================ #
#                            Executable name and options                       #
# ============================================================================ #

# Target name
set(TARGET BabylonCppTests)
message(STATUS "Tests ${TARGET}")

# ============================================================================ #
#                            Sources                                           #
# ============================================================================ #

# Sources
file(GLOB_RECURSE SRC_FILES *.h *.cpp)
set(sources
    ${SRC_FILES}
)

if(NOT OPTION_ENABLE_SIMD)
    list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/math/simd_test.cpp)
endif(NOT OPTION_ENABLE_SIMD)


# ============================================================================ #
#                            Create executable                                 #
# ============================================================================ #

# Build executable
add_executable(${TARGET}
    ${sources}
)
babylon_target_clang_tidy(${TARGET})

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
    ${JSON_HPP_INCLUDE_DIRS}
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

if (NOT MSVC)
    if(NOT TEST_EXCLUDE_FROM_DEFAULT_BUILD)
        add_custom_command (
        TARGET ${TARGET} POST_BUILD
        COMMAND ${TARGET} --gtest_output=xml:${TARGET}.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
        )
    endif()
else()
    add_test(NAME ${TARGET} COMMAND ${TARGET})
    enable_testing()
endif()
