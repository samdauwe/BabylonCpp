#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H

namespace BABYLON {

extern const char* instances300Declaration;

const char* instances300Declaration
  = "#ifdef INSTANCES\n"
    "  in vec4 world0;\n"
    "  in vec4 world1;\n"
    "  in vec4 world2;\n"
    "  in vec4 world3;\n"
    "#else\n"
    "  uniform mat4 world;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H
