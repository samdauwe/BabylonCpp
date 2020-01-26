#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* clipPlaneFragment;

const char* clipPlaneFragment
  = R"ShaderCode(

#ifdef CLIPPLANE
    if (fClipDistance > 0.0)
    {
        discard;
    }
#endif

#ifdef CLIPPLANE2
    if (fClipDistance2 > 0.0)
    {
        discard;
    }
#endif

#ifdef CLIPPLANE3
    if (fClipDistance3 > 0.0)
    {
        discard;
    }
#endif

#ifdef CLIPPLANE4
    if (fClipDistance4 > 0.0)
    {
        discard;
    }
#endif

#ifdef CLIPPLANE5
    if (fClipDistance5 > 0.0)
    {
        discard;
    }
#endif

#ifdef CLIPPLANE6
    if (fClipDistance6 > 0.0)
    {
        discard;
    }
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_FRAGMENT_FX_H
