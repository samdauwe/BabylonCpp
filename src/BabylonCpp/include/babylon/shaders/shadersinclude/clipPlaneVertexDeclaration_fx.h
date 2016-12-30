#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEVERTEXDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEVERTEXDECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneVertexDeclaration;

const char* clipPlaneVertexDeclaration
  = "#ifdef CLIPPLANE\n"
    "  uniform vec4 vClipPlane;\n"
    "  varying float fClipDistance;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIPPLANEVERTEXDECLARATION_FX_H
