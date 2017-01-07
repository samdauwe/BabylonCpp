#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_FX_H

namespace BABYLON {

extern const char* lightFragment;

const char* lightFragment
  = "#ifdef LIGHT{X}\n"
    "  #if defined(LIGHTMAP) && defined(LIGHTMAPEXCLUDED{X}) && defined(LIGHTMAPNOSPECULAR{X})\n"
    "  //No light calculation\n"
    "  #else\n"
    "  #ifndef SPECULARTERM\n"
    "  vec3 vLightSpecular{X} = vec3(0.);\n"
    "  #endif\n"
    "  #ifdef SPOTLIGHT{X}\n"
    "  info = computeSpotLighting(viewDirectionW, normalW, vLightData{X}, vLightDirection{X}, vLightDiffuse{X}.rgb, vLightSpecular{X}, vLightDiffuse{X}.a, glossiness);\n"
    "  #endif\n"
    "  #ifdef HEMILIGHT{X}\n"
    "  info = computeHemisphericLighting(viewDirectionW, normalW, vLightData{X}, vLightDiffuse{X}.rgb, vLightSpecular{X}, vLightGround{X}, glossiness);\n"
    "  #endif\n"
    "  #if defined(POINTLIGHT{X}) || defined(DIRLIGHT{X})\n"
    "  info = computeLighting(viewDirectionW, normalW, vLightData{X}, vLightDiffuse{X}.rgb, vLightSpecular{X}, vLightDiffuse{X}.a, glossiness);\n"
    "  #endif\n"
    "  #endif\n"
    "  #ifdef SHADOW{X}\n"
    "  #ifdef SHADOWVSM{X}\n"
    "  shadow = computeShadowWithVSM(vPositionFromLight{X}, shadowSampler{X}, shadowsInfo{X}.z, shadowsInfo{X}.x);\n"
    "  #else\n"
    "  #ifdef SHADOWPCF{X}\n"
    "  #if defined(POINTLIGHT{X})\n"
    "  shadow = computeShadowWithPCFCube(vLightData{X}.xyz, shadowSampler{X}, shadowsInfo{X}.y, shadowsInfo{X}.z, shadowsInfo{X}.x);\n"
    "  #else\n"
    "  shadow = computeShadowWithPCF(vPositionFromLight{X}, shadowSampler{X}, shadowsInfo{X}.y, shadowsInfo{X}.z, shadowsInfo{X}.x);\n"
    "  #endif\n"
    "  #else\n"
    "  #if defined(POINTLIGHT{X})\n"
    "  shadow = computeShadowCube(vLightData{X}.xyz, shadowSampler{X}, shadowsInfo{X}.x, shadowsInfo{X}.z);\n"
    "  #else\n"
    "  shadow = computeShadow(vPositionFromLight{X}, shadowSampler{X}, shadowsInfo{X}.x, shadowsInfo{X}.z);\n"
    "  #endif\n"
    "  #endif\n"
    "  #endif\n"
    "  #else\n"
    "  shadow = 1.;\n"
    "  #endif\n"
    "  #if defined(LIGHTMAP) && defined(LIGHTMAPEXCLUDED{X})\n"
    "  diffuseBase += lightmapColor * shadow;\n"
    "  #ifdef SPECULARTERM\n"
    "  #ifndef LIGHTMAPNOSPECULAR{X}\n"
    "  specularBase += info.specular * shadow * lightmapColor;\n"
    "  #endif\n"
    "  #endif\n"
    "  #else\n"
    "  diffuseBase += info.diffuse * shadow;\n"
    "  #ifdef SPECULARTERM\n"
    "  specularBase += info.specular * shadow;\n"
    "  #endif\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_FX_H
