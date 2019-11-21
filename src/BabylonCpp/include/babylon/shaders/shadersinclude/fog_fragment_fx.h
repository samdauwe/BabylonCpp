#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOG_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOG_FRAGMENT_FX_H

namespace BABYLON {

extern const char* fogFragment;

const char* fogFragment
  = R"ShaderCode(

#ifdef FOG
    float fog = CalcFogFactor();
    color.rgb = fog * color.rgb + (1.0 - fog) * vFogColor;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOG_FRAGMENT_FX_H
