#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_UBO_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_UBO_DECLARATION_FX_H

namespace BABYLON {

extern const char* defaultUboDeclaration;

const char* defaultUboDeclaration
  = "layout(std140, column_major) uniform;\n"
    "\n"
    "uniform Material\n"
    "{\n"
    "  vec4 diffuseLeftColor;\n"
    "  vec4 diffuseRightColor;\n"
    "  vec4 opacityParts;\n"
    "  vec4 reflectionLeftColor;\n"
    "  vec4 reflectionRightColor;\n"
    "  vec4 refractionLeftColor;\n"
    "  vec4 refractionRightColor;\n"
    "  vec4 emissiveLeftColor; \n"
    "  vec4 emissiveRightColor;\n"
    "  vec2 vDiffuseInfos;\n"
    "  vec2 vAmbientInfos;\n"
    "  vec2 vOpacityInfos;\n"
    "  vec2 vReflectionInfos;\n"
    "  vec3 vReflectionPosition;\n"
    "  vec3 vReflectionSize;\n"
    "  vec2 vEmissiveInfos;\n"
    "  vec2 vLightmapInfos;\n"
    "  vec2 vSpecularInfos;\n"
    "  vec3 vBumpInfos;\n"
    "  mat4 diffuseMatrix;\n"
    "  mat4 ambientMatrix;\n"
    "  mat4 opacityMatrix;\n"
    "  mat4 reflectionMatrix;\n"
    "  mat4 emissiveMatrix;\n"
    "  mat4 lightmapMatrix;\n"
    "  mat4 specularMatrix;\n"
    "  mat4 bumpMatrix; \n"
    "  vec4 vTangentSpaceParams;\n"
    "  mat4 refractionMatrix;\n"
    "  vec4 vRefractionInfos;\n"
    "  vec4 vSpecularColor;\n"
    "  vec3 vEmissiveColor;\n"
    "  vec4 vDiffuseColor;\n"
    "  float pointSize; \n"
    "};\n"
    "\n"
    "uniform Scene {\n"
    "  mat4 viewProjection;\n"
    "  mat4 view;\n"
    "};\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_UBO_DECLARATION_FX_H
