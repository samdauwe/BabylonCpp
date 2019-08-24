#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H

namespace BABYLON {

extern const char* bones300Declaration;

const char* bones300Declaration
  = "#if NUM_BONE_INFLUENCERS > 0\n"
    "  #ifdef BONETEXTURE\n"
    "  uniform sampler2D boneSampler;\n"
    "  #else\n"
    "  uniform mat4 mBones[BonesPerMesh];\n"
    "  #endif  \n"
    "\n"
    "  in vec4 matricesIndices;\n"
    "  in vec4 matricesWeights;\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  in vec4 matricesIndicesExtra;\n"
    "  in vec4 matricesWeightsExtra;\n"
    "  #endif\n"
    "\n"
    "  #ifdef BONETEXTURE\n"
    "  mat4 readMatrixFromRawSampler(sampler2D smp, float index)\n"
    "  {\n"
    "  mat4 result;\n"
    "  float offset = index  * 4.0;  \n"
    "  float dx = 1.0 / boneTextureWidth;\n"
    "\n"
    "  result[0] = texture(smp, vec2(dx * (offset + 0.5), 0.));\n"
    "  result[1] = texture(smp, vec2(dx * (offset + 1.5), 0.));\n"
    "  result[2] = texture(smp, vec2(dx * (offset + 2.5), 0.));\n"
    "  result[3] = texture(smp, vec2(dx * (offset + 3.5), 0.));\n"
    "\n"
    "  return result;\n"
    "  }\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H
