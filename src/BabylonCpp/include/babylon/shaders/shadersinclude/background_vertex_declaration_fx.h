#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundVertexDeclaration;

const char* backgroundVertexDeclaration
  = "uniform mat4 view;\n"
    "uniform mat4 viewProjection;\n"
    "uniform float shadowLevel;\n"
    "\n"
    "#ifdef DIFFUSE\n"
    "uniform mat4 diffuseMatrix;\n"
    "uniform vec2 vDiffuseInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef REFLECTION\n"
    "  uniform vec2 vReflectionInfos;\n"
    "  uniform mat4 reflectionMatrix;\n"
    "  uniform vec3 vReflectionMicrosurfaceInfos;\n"
    "  uniform float fFovMultiplier;\n"
    "#endif\n"
    "\n"
    "#ifdef POINTSIZE\n"
    "uniform float pointSize;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H
