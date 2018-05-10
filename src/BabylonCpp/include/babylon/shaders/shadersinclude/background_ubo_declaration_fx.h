#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundUboDeclaration;

const char* backgroundUboDeclaration
  = "layout(std140, column_major) uniform;\n"
    "\n"
    "uniform Material\n"
    "{\n"
    "  uniform vec4 vPrimaryColor;\n"
    "  uniform vec4 vPrimaryColorShadow;\n"
    "  uniform vec2 vDiffuseInfos;\n"
    "  uniform vec2 vReflectionInfos;\n"
    "  uniform mat4 diffuseMatrix;\n"
    "  uniform mat4 reflectionMatrix;\n"
    "  uniform vec3 vReflectionMicrosurfaceInfos;\n"
    "  uniform float fFovMultiplier;\n"
    "\n"
    "  uniform float pointSize;\n"
    "  uniform float shadowLevel;\n"
    "  uniform float alpha;\n"
    "\n"
    "  #if defined(REFLECTIONFRESNEL) || defined(OPACITYFRESNEL)\n"
    "  uniform vec3 vBackgroundCenter;\n"
    "  #endif\n"
    "\n"
    "  #ifdef REFLECTIONFRESNEL\n"
    "  uniform vec4 vReflectionControl;\n"
    "  #endif\n"
    "};\n"
    "\n"
    "uniform Scene {\n"
    "  mat4 viewProjection;\n"
    "  mat4 view;\n"
    "};\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_UBO_DECLARATION_FX_H
