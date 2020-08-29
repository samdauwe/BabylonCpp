#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H

namespace BABYLON {

extern const char* pbrFragmentExtraDeclaration;

const char* pbrFragmentExtraDeclaration
  = R"ShaderCode(

uniform vec4 vEyePosition;
uniform vec3 vAmbientColor;
uniform vec4 vCameraInfos;

// Input
varying vec3 vPositionW;

#if DEBUGMODE > 0
    uniform vec2 vDebugMode;
    varying vec4 vClipSpacePosition;
#endif

#ifdef MAINUV1
    varying vec2 vMainUV1;
#endif

#ifdef MAINUV2
    varying vec2 vMainUV2;
#endif

#ifdef NORMAL
    varying vec3 vNormalW;
    #if defined(USESPHERICALFROMREFLECTIONMAP) && defined(USESPHERICALINVERTEX)
        varying vec3 vEnvironmentIrradiance;
    #endif
#endif

#ifdef VERTEXCOLOR
    varying vec4 vColor;
#endif

#ifdef PREPASS
    varying vec3 vViewPos;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H
