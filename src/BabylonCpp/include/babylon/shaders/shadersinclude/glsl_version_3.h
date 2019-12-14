#ifndef BABYLONCPP_GLSL_VERSION_3_H
#define BABYLONCPP_GLSL_VERSION_3_H

#ifdef __EMSCRIPTEN__
#define BABYLONCPP_GLSL_VERSION_3 "#version 300 es\n"
#else
#define BABYLONCPP_GLSL_VERSION_3 "#version 330\n"
#endif
//#ifndef __APPLE__
//#define BABYLONCPP_GLSL_VERSION_3 "#version 300 es\n"
//#else
//#define BABYLONCPP_GLSL_VERSION_3 "#version 330\n"
//#endif

#endif //BABYLONCPP_GLSL_VERSION_3_H
