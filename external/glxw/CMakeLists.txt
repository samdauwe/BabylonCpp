cmake_minimum_required(VERSION 2.8)

project(glxw C)

find_package(PythonInterp 2.7.0 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

if(DEFINED GLWT_USE_EGL)
    if(GLWT_USE_EGL)
        set(GLXW_EGL_OPTION "--use-egl")
    else()
        set(GLXW_EGL_OPTION "--no-egl")
    endif()
else()
    set(GLXW_EGL_OPTION )
endif()

# OpenGL
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/GL/glcorearb.h
        COMMAND
            ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
            --api opengl
            --download
            --include ${CMAKE_CURRENT_SOURCE_DIR}/include
            --output ${CMAKE_CURRENT_BINARY_DIR}
            ${GLXW_EGL_OPTION}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/src/glxw.c
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLXW/glxw.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api opengl
        --generate
        --output ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/GL/glcorearb.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_library(glxw STATIC ${CMAKE_CURRENT_BINARY_DIR}/src/glxw.c)

# GLX
find_path(GLX_INCLUDE_DIR GL/glx.h)
mark_as_advanced(GLX_INCLUDE_DIR)
if(GLX_INCLUDE_DIR)
    include_directories(${GLX_INCLUDE_DIR})

    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/include/GL/glxext.h
            COMMAND
                ${PYTHON_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
                --api glx
                --download
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include
                --output ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_glx.c
            ${CMAKE_CURRENT_BINARY_DIR}/include/GLXW/glxw_glx.h
        COMMAND
            ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
            --api glx
            --generate
            --output ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/GL/glxext.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_library(glxw_glx STATIC ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_glx.c)
endif()

# WGL
if(WIN32)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/include/GL/wglext.h
            COMMAND
                ${PYTHON_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
                --api wgl
                --download
                --include ${CMAKE_CURRENT_SOURCE_DIR}/include
                --output ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_wgl.c
            ${CMAKE_CURRENT_BINARY_DIR}/include/GLXW/glxw_wgl.h
        COMMAND
            ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
            --api wgl
            --generate
            --output ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/include/GL/wglext.h
            ${CMAKE_CURRENT_BINARY_DIR}/include/GL/glcorearb.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_library(glxw_wgl STATIC ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_wgl.c)
endif()

# KHR
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/KHR/khrplatform.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api khr
        --download
        --include ${CMAKE_CURRENT_SOURCE_DIR}/include
        --output ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# EGL
find_path(GLXW_EGLPLATFORM_PATH EGL/eglplatform.h PATHS /opt/vc/include/)
if(GLXW_EGLPLATFORM_PATH)
    message("found eglplatform.h")
    include_directories(${GLXW_EGLPLATFORM_PATH})

    set(GLXW_NO_PLATFORM "--with-platform" ${GLXW_EGLPLATFORM_PATH})

    find_path(GLXW_VCOS_PATH vcos_platform_types.h PATHS /opt/vc/include/interface/vcos/pthreads/)
    if(GLXW_VCOS_PATH)
        message("found vcos_platform_types.h (Raspberry PI)")
        include_directories(${GLXW_VCOS_PATH})
    endif()
endif()

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/egl.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/eglext.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/eglplatform.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api egl
        --download
        --include ${CMAKE_CURRENT_SOURCE_DIR}/include
        --output ${CMAKE_CURRENT_BINARY_DIR}
        ${GLXW_NO_PLATFORM}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_egl.c
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLXW/glxw_egl.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api egl
        --generate
        --output ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/egl.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/eglext.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/EGL/eglplatform.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/KHR/khrplatform.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_library(glxw_egl STATIC ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_egl.c)

# GLES2
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2ext.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2platform.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api gles2
        --download
        --include ${CMAKE_CURRENT_SOURCE_DIR}/include
        --output ${CMAKE_CURRENT_BINARY_DIR}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_es2.c
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLXW/glxw_es2.h
    COMMAND
        ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
        --api gles2
        --generate
        --output ${CMAKE_CURRENT_BINARY_DIR}
        ${GLXW_EGL_OPTION}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/glxw_gen.py
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2ext.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/GLES2/gl2platform.h
        ${CMAKE_CURRENT_BINARY_DIR}/include/KHR/khrplatform.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_library(glxw_es2 STATIC ${CMAKE_CURRENT_BINARY_DIR}/src/glxw_es2.c)
