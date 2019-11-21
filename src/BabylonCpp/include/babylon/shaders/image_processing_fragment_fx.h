#ifndef BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H
#define BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H

namespace BABYLON {

extern const char* imageProcessingPixelShader;

const char* imageProcessingPixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

#include<imageProcessingDeclaration>

#include<helperFunctions>

#include<imageProcessingFunctions>

void main(void)
{
    vec4 result = texture2D(textureSampler, vUV);

#ifdef IMAGEPROCESSING
    #ifndef FROMLINEARSPACE
        // Need to move to linear space for subsequent operations.
        result.rgb = toLinearSpace(result.rgb);
    #endif

    result = applyImageProcessing(result);
#else
    // In case where the input is in linear space we at least need to put it back in gamma.
    #ifdef FROMLINEARSPACE
        result = applyImageProcessing(result);
    #endif
#endif

    gl_FragColor = result;
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H
