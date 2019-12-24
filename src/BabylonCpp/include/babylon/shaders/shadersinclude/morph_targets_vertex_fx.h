#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H

namespace BABYLON {

extern const char* morphTargetsVertex;

const char* morphTargetsVertex
  = R"ShaderCode(

#ifdef MORPHTARGETS
    positionUpdated += (position{X} - position) * morphTargetInfluences[{X}];

    #ifdef MORPHTARGETS_NORMAL
    normalUpdated += (normal{X} - normal) * morphTargetInfluences[{X}];
    #endif

    #ifdef MORPHTARGETS_TANGENT
    tangentUpdated.xyz += (tangent{X} - tangent.xyz) * morphTargetInfluences[{X}];
    #endif

    #ifdef MORPHTARGETS_UV
    uvUpdated += (uv_{X} - uv) * morphTargetInfluences[{X}];
    #endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H
