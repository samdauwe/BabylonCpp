#ifndef BABYLON_SHADERS_DISPLAY_PASS_FRAGMENT_FX_H
#define BABYLON_SHADERS_DISPLAY_PASS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* displayPassPixelShader;

const char* displayPassPixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;
uniform sampler2D passSampler;

void main(void)
{
    gl_FragColor = texture2D(passSampler, vUV);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DISPLAY_PASS_FRAGMENT_FX_H
