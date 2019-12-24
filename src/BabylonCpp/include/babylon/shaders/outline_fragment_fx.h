#ifndef BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* outlinePixelShader;

const char* outlinePixelShader
  = R"ShaderCode(

#ifdef LOGARITHMICDEPTH
#extension GL_EXT_frag_depth : enable
#endif
uniform vec4 color;

#ifdef ALPHATEST
varying vec2 vUV;
uniform sampler2D diffuseSampler;
#endif
#include<logDepthDeclaration>

void main(void) {
#ifdef ALPHATEST
    if (texture2D(diffuseSampler, vUV).a < 0.4)
        discard;
#endif
#include<logDepthFragment>
    gl_FragColor = color;
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H
