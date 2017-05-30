#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* defaultVertexDeclaration;

const char* defaultVertexDeclaration
  = "// Uniforms\n"
    "uniform mat4 viewProjection;\n"
    "uniform mat4 view;\n"
    "\n"
    "#ifdef DIFFUSE\n"
    "uniform mat4 diffuseMatrix;\n"
    "uniform vec2 vDiffuseInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef AMBIENT\n"
    "uniform mat4 ambientMatrix;\n"
    "uniform vec2 vAmbientInfos;\n"
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
    "#if defined(SPECULAR) && defined(SPECULARTERM)\n"
    "uniform vec2 vSpecularInfos;\n"
    "uniform mat4 specularMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef BUMP\n"
    "uniform vec3 vBumpInfos;\n"
    "uniform mat4 bumpMatrix;\n"
    "#endif\n"
    "\n"
    "#ifdef POINTSIZE\n"
    "  uniform float pointSize;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_DEFAULT_VERTEX_DECLARATION_FX_H
