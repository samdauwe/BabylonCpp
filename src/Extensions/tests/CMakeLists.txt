if (WIN32)
  message(WARNING "ExtensionsTests needs to be fixed for windows")
else()
  # Target name
  set(TARGET ExtensionsTests)
  message(STATUS "Test ${TARGET}")

  # Sources
  file(GLOB_RECURSE SRC_FILES *.cpp)
  set(sources
      ${SRC_FILES}
  )

  package_add_test(${TARGET} ${sources})
  babylon_target_clang_tidy(${TARGET})

  set_target_properties(${TARGET}
      PROPERTIES ${DEFAULT_PROJECT_OPTIONS}
      FOLDER "${IDE_FOLDER}"
  )

  target_include_directories(${TARGET}
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../include
      ${CMAKE_CURRENT_BINARY_DIR}/../include
  )

  # Libraries
  target_link_libraries(${TARGET} PRIVATE BabylonCpp Extensions)
endif()
