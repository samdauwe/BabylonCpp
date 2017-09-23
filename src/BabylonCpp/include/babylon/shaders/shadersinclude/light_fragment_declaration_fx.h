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
    "  #else\n"
    "  vec3 vLightSpecular{X} = vec3(0.);\n"
    "  #endif\n"
    "  #ifdef SHADOW{X}\n"
    "  #if defined(SHADOWCUBE{X})\n"
    "  uniform samplerCube shadowSampler{X};\n"
    "  #else\n"
    "  varying vec4 vPositionFromLight{X};\n"
    "  varying float vDepthMetric{X};\n"
    "\n"
    "  uniform sampler2D shadowSampler{X};\n"
    "  uniform mat4 lightMatrix{X};\n"
    "  #endif\n"
    "  uniform vec4 shadowsInfo{X};\n"
    "  uniform vec2 depthValues{X};\n"
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
