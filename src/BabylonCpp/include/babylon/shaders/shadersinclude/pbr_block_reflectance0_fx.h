#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE0_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE0_FX_H

namespace BABYLON {

extern const char* pbrBlockReflectance0;

const char* pbrBlockReflectance0
  = R"ShaderCode(

float reflectance = max(max(reflectivityOut.surfaceReflectivityColor.r, reflectivityOut.surfaceReflectivityColor.g), reflectivityOut.surfaceReflectivityColor.b);
vec3 specularEnvironmentR0 = reflectivityOut.surfaceReflectivityColor.rgb;

#ifdef METALLICWORKFLOW
    vec3 specularEnvironmentR90 = vec3(metallicReflectanceFactors.a);
#else
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0);
#endif

// Back Compat
#ifdef ALPHAFRESNEL
    float reflectance90 = fresnelGrazingReflectance(reflectance);
    specularEnvironmentR90 = specularEnvironmentR90 * reflectance90;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_REFLECTANCE0_FX_H
