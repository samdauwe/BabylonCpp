# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Standalone_OSX)

# Sources
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)
file(GLOB RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*)

set(STORYBOARD_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/Main.storyboard)

babylon_add_executable(${TARGET} ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})

target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    ${OPENGL_LIBRARIES}
    BabylonCpp
    MaterialsLibrary
    ProceduralTexturesLibrary
    json_hpp
)

target_compile_definitions(${TARGET} PRIVATE GLES_SILENCE_DEPRECATION)

set_target_properties(${TARGET} PROPERTIES
    MACOSX_BUNDLE YES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/plist.in"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.BabylonCpp.Standalone.OSX"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
    RESOURCE "${STORYBOARD_FILES}"
)
