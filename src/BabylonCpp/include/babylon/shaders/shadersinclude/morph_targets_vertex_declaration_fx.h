#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* morphTargetsVertexDeclaration;

const char* morphTargetsVertexDeclaration
  = "#ifdef MORPHTARGETS\n"
    "  attribute vec3 position{X};\n"
    "\n"
    "  #ifdef MORPHTARGETS_NORMAL\n"
    "  attribute vec3 normal{X};\n"
    "  #endif\n"
    "\n"
    "  #ifdef MORPHTARGETS_TANGENT\n"
    "  attribute vec3 tangent{X};\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_DECLARATION_FX_H
