#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H

namespace BABYLON {

extern const char* lightUboDeclaration;

const char* lightUboDeclaration
  = "#ifdef LIGHT{X}\n"
    "  uniform Light{X}\n"
    "  {\n"
    "  vec4 vLightData;\n"
    "\n"
    "  vec4 vLightDiffuse;\n"
    "  vec3 vLightSpecular;\n"
    "  #ifdef SPOTLIGHT{X}\n"
    "  vec4 vLightDirection;\n"
    "  vec4 vLightFalloff;\n"
    "  #elif defined(POINTLIGHT{X})\n"
    "  vec4 vLightFalloff;\n"
    "  #elif defined(HEMILIGHT{X})\n"
    "  vec3 vLightGround;\n"
    "  #endif\n"
    "  vec4 shadowsInfo;\n"
    "  vec2 depthValues;\n"
    "  } light{X};\n"
    "#ifdef PROJECTEDLIGHTTEXTURE{X}\n"
    "  uniform mat4 textureProjectionMatrix{X};\n"
    "  uniform sampler2D projectionLightSampler{X};\n"
    "#endif\n"
    "#ifdef SHADOW{X}\n"
    "  #if defined(SHADOWCUBE{X})\n"
    "  uniform samplerCube shadowSampler{X};  \n"
    "  #else\n"
    "  varying vec4 vPositionFromLight{X};\n"
    "  varying float vDepthMetric{X};\n"
    "\n"
    "  #if defined(SHADOWPCSS{X})\n"
    "  uniform highp sampler2DShadow shadowSampler{X};\n"
    "  uniform highp sampler2D depthSampler{X};\n"
    "  #elif defined(SHADOWPCF{X})\n"
    "  uniform highp sampler2DShadow shadowSampler{X};\n"
    "  #else\n"
    "  uniform sampler2D shadowSampler{X};\n"
    "  #endif\n"
    "  uniform mat4 lightMatrix{X};\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LIGHT_UBO_DECLARATION_FX_H
