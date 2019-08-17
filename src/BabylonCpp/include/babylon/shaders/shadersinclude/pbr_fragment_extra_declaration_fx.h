#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H

namespace BABYLON {

extern const char* pbrFragmentExtraDeclaration;

const char* pbrFragmentExtraDeclaration
  = "uniform vec4 vEyePosition;\n"
    "uniform vec3 vAmbientColor;\n"
    "uniform vec4 vCameraInfos;\n"
    "\n"
    "// Input\n"
    "varying vec3 vPositionW;\n"
    "\n"
    "#if DEBUGMODE > 0\n"
    "  uniform vec2 vDebugMode;\n"
    "  varying vec4 vClipSpacePosition;\n"
    "#endif\n"
    "\n"
    "#ifdef MAINUV1\n"
    "  varying vec2 vMainUV1;\n"
    "#endif \n"
    "\n"
    "#ifdef MAINUV2 \n"
    "  varying vec2 vMainUV2;\n"
    "#endif \n"
    "\n"
    "#ifdef NORMAL\n"
    "  varying vec3 vNormalW;\n"
    "  #if defined(USESPHERICALFROMREFLECTIONMAP) && defined(USESPHERICALINVERTEX)\n"
    "  varying vec3 vEnvironmentIrradiance;\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "  varying vec4 vColor;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_FRAGMENT_EXTRA_DECLARATION_FX_H
