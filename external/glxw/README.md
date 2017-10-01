GLXW OpenGL loader
==================

GLXW is an OpenGL loader that dynamically links with OpenGL at runtime.
It supports OpenGL core profile, OpenGL ES 2, OpenGL ES 3, WGL, GLX and EGL.

GLXW is a Python script that downloads OpenGL header files from official
locations, parses them and generates the loader library headers and sources.

Basic use
---------

1.  Run the glxw_gen.py script to download GL headers, parse them and generate
    loader.
    See --help for command line options.

        $ python glxw_gen.py

2.  Include glxw.c to your project sources and add glxw to your include path.
    Link with OpenGL libraries and/or EGL libraries as applicable.
    On Linux and other Unix platforms that use dlopen(), link with -ldl.
    Alternatively, CMake can be used to generate and build GLXW.

3.  In your project, #include \<GLXW/glxw.h\> and call glxwInit() when an
    OpenGL context is active.
    You do not have to include standard OpenGL headers, they are included from
    glxw.h.

        #include <GLXW/glxw.h>

        if(glxwInit() != 0)
            handle_error();

4.  If you use CMake and intend to rebuild a lot or use GLXW as a CMake
    subproject, copy downloaded headers to glxw/include to avoid re-downloading
    after every "make clean".

        $ cp -R build/include/{GL,GLES2,GLES3,EGL,KHR} include/

License
-------

GLXW is licensed under the zlib license. See file LICENSE for details.
