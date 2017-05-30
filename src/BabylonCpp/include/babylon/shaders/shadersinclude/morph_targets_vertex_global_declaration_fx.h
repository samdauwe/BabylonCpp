#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H

namespace BABYLON {

extern const char* morphTargetsVertexGlobalDeclaration;

const char* morphTargetsVertexGlobalDeclaration
  = "#ifdef MORPHTARGETS\n"
    "  uniform float morphTargetInfluences[NUM_MORPH_INFLUENCERS];\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_GLOBAL_DECLARATION_FX_H
