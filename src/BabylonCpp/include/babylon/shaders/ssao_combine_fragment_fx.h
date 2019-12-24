#ifndef BABYLON_SHADERS_SSAO_COMBINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_SSAO_COMBINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* ssaoCombinePixelShader;

const char* ssaoCombinePixelShader
  = R"ShaderCode(

uniform sampler2D textureSampler;
uniform sampler2D originalColor;
uniform vec4 viewport;

varying vec2 vUV;

void main(void) {
    vec4 ssaoColor = texture2D(textureSampler, viewport.xy + vUV * viewport.zw);
    vec4 sceneColor = texture2D(originalColor, vUV);

    gl_FragColor = sceneColor * ssaoColor;
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SSAO_COMBINE_FRAGMENT_FX_H
