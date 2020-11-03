# Configure build environment
include(../../cmake/BuildEnvironment.cmake)

# Target name
set(TARGET Standalone_iOS)

# Sources
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.*)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)
file(GLOB RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*)

set(STORYBOARD_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/Main.storyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/LaunchScreen.storyboard)

babylon_add_executable(${TARGET} ${HEADER_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})

target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    BabylonCpp
    MaterialsLibrary
    # TODO: Fix issue when setting to static for iOS
    # ProceduralTexturesLibrary
    json_hpp
)

target_compile_definitions(${TARGET} PRIVATE GLES_SILENCE_DEPRECATION GLES3=1)

set_target_properties(${TARGET} PROPERTIES
    MACOSX_BUNDLE true
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/plist.in"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 12.0
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.BabylonCpp.Standalone.iOS"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    RESOURCE "${STORYBOARD_FILES}"
)
