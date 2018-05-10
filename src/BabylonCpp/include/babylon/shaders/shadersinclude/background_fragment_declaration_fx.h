#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundFragmentDeclaration;

const char* backgroundFragmentDeclaration
  = "    uniform vec4 vPrimaryColor;\n"
    "#ifdef USEHIGHLIGHTANDSHADOWCOLORS\n"
    "  uniform vec4 vPrimaryColorShadow;\n"
    "#endif\n"
    "  uniform float shadowLevel;\n"
    "  uniform float alpha;\n"
    "\n"
    "#ifdef DIFFUSE\n"
    "  uniform vec2 vDiffuseInfos;\n"
    "#endif\n"
    "\n"
    "#ifdef REFLECTION\n"
    "  uniform vec2 vReflectionInfos;\n"
    "  uniform mat4 reflectionMatrix;\n"
    "  uniform vec3 vReflectionMicrosurfaceInfos;\n"
    "#endif\n"
    "\n"
    "#if defined(REFLECTIONFRESNEL) || defined(OPACITYFRESNEL)\n"
    "  uniform vec3 vBackgroundCenter;\n"
    "#endif\n"
    "\n"
    "#ifdef REFLECTIONFRESNEL\n"
    "  uniform vec4 vReflectionControl;\n"
    "#endif\n"
    "\n"
    "#if defined(REFLECTIONMAP_SPHERICAL) || defined(REFLECTIONMAP_PROJECTION) || defined(REFRACTION)\n"
    "uniform mat4 view;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H
