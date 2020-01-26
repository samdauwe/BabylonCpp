#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H

namespace BABYLON {

extern const char* clipPlaneFragmentDeclaration2;

const char* clipPlaneFragmentDeclaration2
  = R"ShaderCode(

#ifdef CLIPPLANE
    in float fClipDistance;
#endif

#ifdef CLIPPLANE2
    in float fClipDistance2;
#endif

#ifdef CLIPPLANE3
    in float fClipDistance3;
#endif

#ifdef CLIPPLANE4
    in float fClipDistance4;
#endif

#ifdef CLIPPLANE5
    in float fClipDistance5;
#endif

#ifdef CLIPPLANE6
    in float fClipDistance6;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION2_FX_H
