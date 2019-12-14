// In Wasm / emscripten mode, we do not use glad

#ifndef __EMSCRIPTEN__
#error "This header is reserved for emscripten"
#endif

#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>


