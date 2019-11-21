#ifndef BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H
#define BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* lensFlarePixelShader;

const char* lensFlarePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;

// Color
uniform vec4 color;

void main(void) {
    vec4 baseColor = texture2D(textureSampler, vUV);

    gl_FragColor = baseColor * color;
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H
