#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BONESDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BONESDECLARATION_FX_H

namespace BABYLON {

extern const char* bonesDeclaration;

const char* bonesDeclaration
  = "#if NUM_BONE_INFLUENCERS > 0\n"
    "  uniform mat4 mBones[BonesPerMesh];\n"
    "\n"
    "  attribute vec4 matricesIndices;\n"
    "  attribute vec4 matricesWeights;\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  attribute vec4 matricesIndicesExtra;\n"
    "  attribute vec4 matricesWeightsExtra;\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BONESDECLARATION_FX_H
