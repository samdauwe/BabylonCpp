#ifndef BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
#define BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* blackAndWhitePixelShader;

const char* blackAndWhitePixelShader
  = R"ShaderCode(

// Samplers
varying vec2 vUV;
uniform sampler2D textureSampler;
uniform float degree;

void main(void)
{
    vec3 color = texture2D(textureSampler, vUV).rgb;
    float luminance = dot(color, vec3(0.3, 0.59, 0.11));
    vec3 blackAndWhite = vec3(luminance, luminance, luminance);
    gl_FragColor = vec4(color - ((color - blackAndWhite) * degree), 1.0);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
