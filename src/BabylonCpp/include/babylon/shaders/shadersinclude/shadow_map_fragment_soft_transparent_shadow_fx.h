#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_FRAGMENT_SOFT_TRANSPARENT_SHADOW_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_FRAGMENT_SOFT_TRANSPARENT_SHADOW_FX_H

namespace BABYLON {

extern const char* shadowMapFragmentSoftTransparentShadow;

const char* shadowMapFragmentSoftTransparentShadow
  = R"ShaderCode(

#if SM_SOFTTRANSPARENTSHADOW == 1
    if ((bayerDither8(floor(mod(gl_FragCoord.xy, 8.0)))) / 64.0 >= softTransparentShadowSM * alpha) discard;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOW_MAP_FRAGMENT_SOFT_TRANSPARENT_SHADOW_FX_H
