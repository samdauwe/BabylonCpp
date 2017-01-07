#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H

namespace BABYLON {

extern const char* clipPlaneVertex;

const char* clipPlaneVertex
  = "#ifdef CLIPPLANE\n"
    "  fClipDistance = dot(worldPos, vClipPlane);\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
