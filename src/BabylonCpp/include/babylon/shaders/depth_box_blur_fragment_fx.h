#ifndef BABYLON_SHADERS_DEPTH_BOX_BLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_DEPTH_BOX_BLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* depthBoxBlurPixelShader;

const char* depthBoxBlurPixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

// Parameters
uniform vec2 screenSize;

void main(void)
{
    vec4 colorDepth = vec4(0.0);

    for (int x = -OFFSET; x <= OFFSET; x++)
        for (int y = -OFFSET; y <= OFFSET; y++)
            colorDepth += texture2D(textureSampler, vUV + vec2(x, y) / screenSize);

    gl_FragColor = (colorDepth / float((OFFSET * 2 + 1) * (OFFSET * 2 + 1)));
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTH_BOX_BLUR_FRAGMENT_FX_H
