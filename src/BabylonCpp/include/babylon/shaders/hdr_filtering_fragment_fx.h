#ifndef BABYLON_SHADERS_HDR_FILTERING_FRAGMENT_FX_H
#define BABYLON_SHADERS_HDR_FILTERING_FRAGMENT_FX_H

namespace BABYLON {

extern const char* hdrFilteringPixelShader;

const char* hdrFilteringPixelShader
  = R"ShaderCode(

#include<helperFunctions>
#include<importanceSampling>
#include<pbrBRDFFunctions>
#include<hdrFilteringFunctions>

uniform float alphaG;
uniform samplerCube inputTexture;
uniform vec2 vFilteringInfo;
uniform float hdrScale;

varying vec3 direction;

void main() {
    vec3 color = radiance(alphaG, inputTexture, direction, vFilteringInfo);

    gl_FragColor = vec4(color * hdrScale, 1.0);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_HDR_FILTERING_FRAGMENT_FX_H
