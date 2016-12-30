#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENTDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENTDECLARATION_FX_H

namespace BABYLON {

extern const char* fogFragmentDeclaration;

const char* fogFragmentDeclaration
  = "#ifdef FOG\n"
    "\n"
    "#define FOGMODE_NONE    0.\n"
    "#define FOGMODE_EXP     1.\n"
    "#define FOGMODE_EXP2    2.\n"
    "#define FOGMODE_LINEAR  3.\n"
    "#define E 2.71828\n"
    "\n"
    "uniform vec4 vFogInfos;\n"
    "uniform vec3 vFogColor;\n"
    "varying float fFogDistance;\n"
    "\n"
    "float CalcFogFactor()\n"
    "{\n"
    "  float fogCoeff = 1.0;\n"
    "  float fogStart = vFogInfos.y;\n"
    "  float fogEnd = vFogInfos.z;\n"
    "  float fogDensity = vFogInfos.w;\n"
    "\n"
    "  if (FOGMODE_LINEAR == vFogInfos.x)\n"
    "  {\n"
    "  fogCoeff = (fogEnd - fFogDistance) / (fogEnd - fogStart);\n"
    "  }\n"
    "  else if (FOGMODE_EXP == vFogInfos.x)\n"
    "  {\n"
    "  fogCoeff = 1.0 / pow(E, fFogDistance * fogDensity);\n"
    "  }\n"
    "  else if (FOGMODE_EXP2 == vFogInfos.x)\n"
    "  {\n"
    "  fogCoeff = 1.0 / pow(E, fFogDistance * fFogDistance * fogDensity * fogDensity);\n"
    "  }\n"
    "\n"
    "  return clamp(fogCoeff, 0.0, 1.0);\n"
    "}\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOGFRAGMENTDECLARATION_FX_H
