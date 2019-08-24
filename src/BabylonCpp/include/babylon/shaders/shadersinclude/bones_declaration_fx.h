#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BONES_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BONES_DECLARATION_FX_H

namespace BABYLON {

extern const char* bonesDeclaration;

const char* bonesDeclaration
  = "#if NUM_BONE_INFLUENCERS > 0\n"
    "  #ifdef BONETEXTURE\n"
    "  uniform sampler2D boneSampler;\n"
    "  uniform float boneTextureWidth;\n"
    "  #else\n"
    "  uniform mat4 mBones[BonesPerMesh];\n"
    "  #endif  \n"
    "\n"
    "  attribute vec4 matricesIndices;\n"
    "  attribute vec4 matricesWeights;\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  attribute vec4 matricesIndicesExtra;\n"
    "  attribute vec4 matricesWeightsExtra;\n"
    "  #endif\n"
    "\n"
    "  #ifdef BONETEXTURE\n"
    "  mat4 readMatrixFromRawSampler(sampler2D smp, float index)\n"
    "  {\n"
    "  float offset = index  * 4.0;  \n"
    "  float dx = 1.0 / boneTextureWidth;\n"
    "\n"
    "  vec4 m0 = texture2D(smp, vec2(dx * (offset + 0.5), 0.));\n"
    "  vec4 m1 = texture2D(smp, vec2(dx * (offset + 1.5), 0.));\n"
    "  vec4 m2 = texture2D(smp, vec2(dx * (offset + 2.5), 0.));\n"
    "  vec4 m3 = texture2D(smp, vec2(dx * (offset + 3.5), 0.));\n"
    "\n"
    "  return mat4(m0, m1, m2, m3);\n"
    "  }\n"
    "  #endif\n"
    "\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BONES_DECLARATION_FX_H
