#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundFragmentDeclaration;

const char* backgroundFragmentDeclaration
  = R"ShaderCode(

    uniform vec4 vPrimaryColor;
#ifdef USEHIGHLIGHTANDSHADOWCOLORS
    uniform vec4 vPrimaryColorShadow;
#endif
    uniform float shadowLevel;
    uniform float alpha;

#ifdef DIFFUSE
    uniform vec2 vDiffuseInfos;
#endif

#ifdef REFLECTION
    uniform vec2 vReflectionInfos;
    uniform mat4 reflectionMatrix;
    uniform vec3 vReflectionMicrosurfaceInfos;
#endif

#if defined(REFLECTIONFRESNEL) || defined(OPACITYFRESNEL)
    uniform vec3 vBackgroundCenter;
#endif

#ifdef REFLECTIONFRESNEL
    uniform vec4 vReflectionControl;
#endif

#if defined(REFLECTIONMAP_SPHERICAL) || defined(REFLECTIONMAP_PROJECTION) || defined(REFRACTION)
uniform mat4 view;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_FRAGMENT_DECLARATION_FX_H
