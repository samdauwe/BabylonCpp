#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* lightFragmentDeclaration;

const char* lightFragmentDeclaration
  = "#ifdef LIGHT{X}\n"
    "  uniform vec4 vLightData{X};\n"
    "  uniform vec4 vLightDiffuse{X};\n"
    "  #ifdef SPECULARTERM\n"
    "  uniform vec3 vLightSpecular{X};\n"
    "  #endif\n"
    "  #ifdef SHADOW{X}\n"
    "  #if defined(SPOTLIGHT{X}) || defined(DIRLIGHT{X})\n"
    "  varying vec4 vPositionFromLight{X};\n"
    "  uniform sampler2D shadowSampler{X};\n"
    "  #else\n"
    "  uniform samplerCube shadowSampler{X};\n"
    "  #endif\n"
    "  uniform vec3 shadowsInfo{X};\n"
    "  #endif\n"
    "  #ifdef SPOTLIGHT{X}\n"
    "  uniform vec4 vLightDirection{X};\n"
    "  #endif\n"
    "  #ifdef HEMILIGHT{X}\n"
    "  uniform vec3 vLightGround{X};\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_FRAGMENT_DECLARATION_FX_H
