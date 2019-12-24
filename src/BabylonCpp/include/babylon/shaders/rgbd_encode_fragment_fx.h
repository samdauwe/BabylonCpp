#ifndef BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H
#define BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* rgbdEncodePixelShader;

const char* rgbdEncodePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

#include<helperFunctions>

void main(void)
{
    gl_FragColor = toRGBD(texture2D(textureSampler, vUV).rgb);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H
