#ifndef BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H
#define BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H

namespace BABYLON {

extern const char* vrMultiviewToSingleviewPixelShader;

const char* vrMultiviewToSingleviewPixelShader
  = R"ShaderCode(

#ifdef GL_ES
    precision mediump sampler2DArray;
#endif

varying vec2 vUV;
uniform sampler2DArray multiviewSampler;
uniform int imageIndex;

void main(void)
{
    gl_FragColor = texture(multiviewSampler, vec3(vUV, imageIndex));
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H
