#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H

namespace BABYLON {

extern const char* lightUboDeclaration;

const char* lightUboDeclaration
  = "#ifdef LIGHT{X}\n"
    "  uniform Light{X}\n"
    "  {\n"
    "  vec4 vLightData;\n"
    "  vec4 vLightDiffuse;\n"
    "  vec3 vLightSpecular;\n"
    "  #ifdef SPOTLIGHT{X}\n"
    "  vec4 vLightDirection;\n"
    "  #endif\n"
    "  #ifdef HEMILIGHT{X}\n"
    "  vec3 vLightGround;\n"
    "  #endif\n"
    "  vec3 shadowsInfo;\n"
    "  } light{X};\n"
    "\n"
    "#ifdef SHADOW{X}\n"
    "  #if defined(SPOTLIGHT{X}) || defined(DIRLIGHT{X})\n"
    "  varying vec4 vPositionFromLight{X};\n"
    "  uniform sampler2D shadowSampler{X};\n"
    "  #else\n"
    "  uniform samplerCube shadowSampler{X};\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H
