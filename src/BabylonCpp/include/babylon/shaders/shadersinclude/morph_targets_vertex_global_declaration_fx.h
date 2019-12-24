#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H

namespace BABYLON {

extern const char* morphTargetsVertexGlobalDeclaration;

const char* morphTargetsVertexGlobalDeclaration
  = R"ShaderCode(

#ifdef MORPHTARGETS
    uniform float morphTargetInfluences[NUM_MORPH_INFLUENCERS];
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H
