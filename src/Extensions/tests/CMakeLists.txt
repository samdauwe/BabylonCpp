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

  babylon_add_test(${TARGET} ${sources})

  target_include_directories(${TARGET}
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../include
      ${CMAKE_CURRENT_BINARY_DIR}/../include
  )

  # Libraries
  target_link_libraries(${TARGET} PRIVATE BabylonCpp Extensions)
endif()
