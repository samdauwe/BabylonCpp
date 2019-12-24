#ifndef BABYLON_SHADERS_PASS_FRAGMENT_FX_H
#define BABYLON_SHADERS_PASS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* passPixelShader;

const char* passPixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

void main(void)
{
    gl_FragColor = texture2D(textureSampler, vUV);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PASS_FRAGMENT_FX_H
