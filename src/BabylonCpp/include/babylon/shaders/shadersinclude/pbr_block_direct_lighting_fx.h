#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_DIRECT_LIGHTING_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_DIRECT_LIGHTING_FX_H

namespace BABYLON {

extern const char* pbrBlockDirectLighting;

const char* pbrBlockDirectLighting
  = R"ShaderCode(

vec3 diffuseBase = vec3(0., 0., 0.);
#ifdef SPECULARTERM
    vec3 specularBase = vec3(0., 0., 0.);
#endif
#ifdef CLEARCOAT
    vec3 clearCoatBase = vec3(0., 0., 0.);
#endif
#ifdef SHEEN
    vec3 sheenBase = vec3(0., 0., 0.);
#endif

// Direct Lighting Variables
preLightingInfo preInfo;
lightingInfo info;
float shadow = 1.; // 1 - shadowLevel

#if defined(CLEARCOAT) && defined(CLEARCOAT_TINT)
    vec3 absorption = vec3(0.);
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_DIRECT_LIGHTING_FX_H
