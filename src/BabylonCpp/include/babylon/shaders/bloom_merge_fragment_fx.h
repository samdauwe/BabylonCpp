#ifndef BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* bloomMergePixelShader;

const char* bloomMergePixelShader
  = R"ShaderCode(

uniform sampler2D textureSampler;
uniform sampler2D bloomBlur;

varying vec2 vUV;
uniform float bloomWeight;

void main(void)
{
    gl_FragColor = texture2D(textureSampler, vUV);
    vec3 blurred = texture2D(bloomBlur, vUV).rgb;
    gl_FragColor.rgb = gl_FragColor.rgb + (blurred.rgb * bloomWeight);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H
