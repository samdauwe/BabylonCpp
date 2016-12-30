#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOGVERTEXDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOGVERTEXDECLARATION_FX_H

namespace BABYLON {

extern const char* fogVertexDeclaration;

const char* fogVertexDeclaration
  = "#ifdef FOG\n"
    "  varying float fFogDistance;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOGVERTEXDECLARATION_FX_H
