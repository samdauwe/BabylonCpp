set(TARGET BabylonCppTests)
message(STATUS "Tests ${TARGET}")

file(GLOB_RECURSE sources *.h *.cpp)
if(NOT OPTION_ENABLE_SIMD)
    list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/math/simd_test.cpp)
endif(NOT OPTION_ENABLE_SIMD)

package_add_test(${TARGET} ${sources})
babylon_target_clang_tidy(${TARGET})
target_link_libraries(${TARGET} PRIVATE BabylonCpp json_hpp)
