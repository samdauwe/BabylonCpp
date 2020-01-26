#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneFragmentDeclaration;

const char* clipPlaneFragmentDeclaration
  = R"ShaderCode(

#ifdef CLIPPLANE
    varying float fClipDistance;
#endif

#ifdef CLIPPLANE2
    varying float fClipDistance2;
#endif

#ifdef CLIPPLANE3
    varying float fClipDistance3;
#endif

#ifdef CLIPPLANE4
    varying float fClipDistance4;
#endif

#ifdef CLIPPLANE5
    varying float fClipDistance5;
#endif

#ifdef CLIPPLANE6
    varying float fClipDistance6;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_DECLARATION_FX_H
