#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneVertexDeclaration;

const char* clipPlaneVertexDeclaration
  = "#ifdef CLIPPLANE\n"
    "  uniform vec4 vClipPlane;\n"
    "  varying float fClipDistance;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  uniform vec4 vClipPlane2;\n"
    "  varying float fClipDistance2;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  uniform vec4 vClipPlane3;\n"
    "  varying float fClipDistance3;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  uniform vec4 vClipPlane4;\n"
    "  varying float fClipDistance4;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H
