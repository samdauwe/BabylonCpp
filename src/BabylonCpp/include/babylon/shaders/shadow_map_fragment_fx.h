#ifndef BABYLON_SHADERS_SHADOW_MAP_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADOW_MAP_FRAGMENT_FX_H

namespace BABYLON {

extern const char* shadowMapPixelShader;

const char* shadowMapPixelShader
  = R"ShaderCode(

#include<shadowMapFragmentDeclaration>

#ifdef ALPHATEST
varying vec2 vUV;
uniform sampler2D diffuseSampler;
#endif

#include<clipPlaneFragmentDeclaration>

void main(void)
{
#include<clipPlaneFragment>

#ifdef ALPHATEST
    float alphaFromAlphaTexture = texture2D(diffuseSampler, vUV).a;
    if (alphaFromAlphaTexture < 0.4)
        discard;
#endif

#if SM_SOFTTRANSPARENTSHADOW == 1
    #ifdef ALPHATEST
        if ((bayerDither8(floor(mod(gl_FragCoord.xy, 8.0)))) / 64.0 >= softTransparentShadowSM * alphaFromAlphaTexture) discard;
    #else
        if ((bayerDither8(floor(mod(gl_FragCoord.xy, 8.0)))) / 64.0 >= softTransparentShadowSM) discard;
    #endif
#endif

#include<shadowMapFragment>
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADOW_MAP_FRAGMENT_FX_H
