#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H

namespace BABYLON {

extern const char* clipPlaneVertex;

const char* clipPlaneVertex
  = R"ShaderCode(

#ifdef CLIPPLANE
    fClipDistance = dot(worldPos, vClipPlane);
#endif

#ifdef CLIPPLANE2
    fClipDistance2 = dot(worldPos, vClipPlane2);
#endif

#ifdef CLIPPLANE3
    fClipDistance3 = dot(worldPos, vClipPlane3);
#endif

#ifdef CLIPPLANE4
    fClipDistance4 = dot(worldPos, vClipPlane4);
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_FX_H
