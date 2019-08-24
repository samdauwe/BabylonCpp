#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BONES_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BONES_VERTEX_FX_H

namespace BABYLON {

extern const char* bonesVertex;

const char* bonesVertex
  = "#if NUM_BONE_INFLUENCERS > 0\n"
    "  mat4 influence;\n"
    "\n"
    "#ifdef BONETEXTURE\n"
    "  influence = readMatrixFromRawSampler(boneSampler, matricesIndices[0]) * matricesWeights[0];\n"
    "\n"
    "  #if NUM_BONE_INFLUENCERS > 1\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndices[1]) * matricesWeights[1];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 2\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndices[2]) * matricesWeights[2];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 3\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndices[3]) * matricesWeights[3];\n"
    "  #endif  \n"
    "\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndicesExtra[0]) * matricesWeightsExtra[0];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 5\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndicesExtra[1]) * matricesWeightsExtra[1];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 6\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndicesExtra[2]) * matricesWeightsExtra[2];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 7\n"
    "  influence += readMatrixFromRawSampler(boneSampler, matricesIndicesExtra[3]) * matricesWeightsExtra[3];\n"
    "  #endif  \n"
    "#else  \n"
    "  influence = mBones[int(matricesIndices[0])] * matricesWeights[0];\n"
    "\n"
    "  #if NUM_BONE_INFLUENCERS > 1\n"
    "  influence += mBones[int(matricesIndices[1])] * matricesWeights[1];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 2\n"
    "  influence += mBones[int(matricesIndices[2])] * matricesWeights[2];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 3\n"
    "  influence += mBones[int(matricesIndices[3])] * matricesWeights[3];\n"
    "  #endif  \n"
    "\n"
    "  #if NUM_BONE_INFLUENCERS > 4\n"
    "  influence += mBones[int(matricesIndicesExtra[0])] * matricesWeightsExtra[0];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 5\n"
    "  influence += mBones[int(matricesIndicesExtra[1])] * matricesWeightsExtra[1];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 6\n"
    "  influence += mBones[int(matricesIndicesExtra[2])] * matricesWeightsExtra[2];\n"
    "  #endif  \n"
    "  #if NUM_BONE_INFLUENCERS > 7\n"
    "  influence += mBones[int(matricesIndicesExtra[3])] * matricesWeightsExtra[3];\n"
    "  #endif  \n"
    "#endif\n"
    "\n"
    "  finalWorld = finalWorld * influence;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BONES_VERTEX_FX_H
