#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEXDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEXDECLARATION_FX_H

namespace BABYLON {

extern const char* shadowsVertexDeclaration;

const char* shadowsVertexDeclaration
  = "#ifdef SHADOWS\n"
    "  #if defined(SPOTLIGHT{X}) || defined(DIRLIGHT{X})\n"
    "  uniform mat4 lightMatrix{X};\n"
    "  varying vec4 vPositionFromLight{X};\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEXDECLARATION_FX_H
