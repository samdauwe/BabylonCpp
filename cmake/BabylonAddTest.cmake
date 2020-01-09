macro(babylon_add_test TESTNAME)
    message("babylon_add_test ${TESTNAME}")
    add_executable(${TESTNAME} ${ARGN})
    # Create namespaced alias
    add_executable(${META_PROJECT_NAME}::${TESTNAME} ALIAS ${TESTNAME})

    target_link_libraries(${TESTNAME} PRIVATE gtest gmock gtest_main)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
    babylon_target_clang_tidy(${TESTNAME})
endmacro()
