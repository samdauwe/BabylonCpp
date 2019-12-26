option(BABYLON_BUILD_BENCHMARK    "Add benchmark to tests" OFF)

if (BABYLON_BUILD_BENCHMARK)
    set(TARGET BabylonCppLibBenchmarks)
    message(STATUS "Benchmarks ${TARGET}")

    file(GLOB_RECURSE SRC_FILES *.cpp)
    babylon_add_test(${TARGET} ${SRC_FILES})
    babylon_target_clang_tidy(${TARGET})

    # Libraries
    target_link_libraries(${TARGET} PRIVATE BabylonCpp)
endif()
