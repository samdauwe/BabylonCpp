#ifndef BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H
#define BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* rgbdDecodePixelShader;

const char* rgbdDecodePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

#include<helperFunctions>

void main(void)
{
    gl_FragColor = vec4(fromRGBD(texture2D(textureSampler, vUV)), 1.0);
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H
