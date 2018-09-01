#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H

namespace BABYLON {

extern const char* clipPlaneVertex;

const char* clipPlaneVertex
  = "#ifdef CLIPPLANE\n"
    "  fClipDistance = dot(worldPos, vClipPlane);\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE2\n"
    "  fClipDistance2 = dot(worldPos, vClipPlane2);\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE3\n"
    "  fClipDistance3 = dot(worldPos, vClipPlane3);\n"
    "#endif\n"
    "\n"
    "#ifdef CLIPPLANE4\n"
    "  fClipDistance4 = dot(worldPos, vClipPlane4);\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
