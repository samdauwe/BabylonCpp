set(TARGET BabylonCppTests)
message(STATUS "Tests ${TARGET}")

file(GLOB_RECURSE sources *.h *.cpp)
package_add_test(${TARGET} ${sources})
babylon_target_clang_tidy(${TARGET})
target_link_libraries(${TARGET} PRIVATE BabylonCpp json_hpp)
