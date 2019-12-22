include(../../cmake/BuildEnvironment.cmake)

set(TARGET lab)
file(GLOB_RECURSE sources *.h *.cpp)
add_executable(lab ${sources})
target_link_libraries(lab PRIVATE BabylonCpp promise_cpp)
