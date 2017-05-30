#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* pbrVertexDeclaration;

const char* pbrVertexDeclaration
  = "uniform mat4 view;\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "#ifdef ALBEDO\n"
    "uniform mat4 albedoMatrix;\n"
    "uniform vec2 vAlbedoInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef AMBIENT\n"
    "uniform mat4 ambientMatrix;\n"
    "uniform vec3 vAmbientInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef OPACITY\n"
    "uniform mat4 opacityMatrix;\n"
    "uniform vec2 vOpacityInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef EMISSIVE\n"
    "uniform vec2 vEmissiveInfos;\n"
    "uniform mat4 emissiveMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef LIGHTMAP\n"
    "uniform vec2 vLightmapInfos;\n"
    "uniform mat4 lightmapMatrix;\n"
    "#endif\n"
    "\n"
    "#if defined(REFLECTIVITY) || defined(METALLICWORKFLOW) \n"
    "uniform vec3 vReflectivityInfos;\n"
    "uniform mat4 reflectivityMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef MICROSURFACEMAP\n"
    "uniform vec2 vMicroSurfaceSamplerInfos;\n"
    "uniform mat4 microSurfaceSamplerMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef BUMP\n"
    "uniform vec3 vBumpInfos;\n"
    "uniform mat4 bumpMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef POINTSIZE\n"
    "uniform float pointSize;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_VERTEX_DECLARATION_FX_H
