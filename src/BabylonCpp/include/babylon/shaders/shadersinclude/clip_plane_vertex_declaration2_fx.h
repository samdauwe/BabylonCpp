#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION2_FX_H

namespace BABYLON {

extern const char* clipPlaneVertexDeclaration2;

const char* clipPlaneVertexDeclaration2
  = "#ifdef CLIPPLANE\n"
    "  uniform vec4 vClipPlane;\n"
    "  out float fClipDistance;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  uniform vec4 vClipPlane2;\n"
    "  out float fClipDistance2;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  uniform vec4 vClipPlane3;\n"
    "  out float fClipDistance3;\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  uniform vec4 vClipPlane4;\n"
    "  out float fClipDistance4;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION2_FX_H
