#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FRESNEL_FUNCTION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FRESNEL_FUNCTION_FX_H

namespace BABYLON {

extern const char* fresnelFunction;

const char* fresnelFunction
  = R"ShaderCode(

#ifdef FRESNEL
    float computeFresnelTerm(vec3 viewDirection, vec3 worldNormal, float bias, float power)
    {
        float fresnelTerm = pow(bias + abs(dot(viewDirection, worldNormal)), power);
        return clamp(fresnelTerm, 0., 1.);
    }
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FRESNEL_FUNCTION_FX_H
