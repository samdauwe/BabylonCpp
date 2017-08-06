#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H

namespace BABYLON {

extern const char* bones300Declaration;

const char* bones300Declaration
  = "#if NUM_BONE_INFLUENCERS > 0\n"
    "  uniform mat4 mBones[BonesPerMesh];\n"
    "\n"
    "  in vec4 matricesIndices;\n"
    "  in vec4 matricesWeights;\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  in vec4 matricesIndicesExtra;\n"
    "  in vec4 matricesWeightsExtra;\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BONES300_DECLARATION_FX_H
