#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H

namespace BABYLON {

extern const char* morphTargetsVertex;

const char* morphTargetsVertex
  = "#ifdef MORPHTARGETS\n"
    "  positionUpdated += (position{X} - position) * morphTargetInfluences[{X}];\n"
    "  \n"
    "  #ifdef MORPHTARGETS_NORMAL\n"
    "  normalUpdated += (normal{X} - normal) * morphTargetInfluences[{X}];\n"
    "  #endif\n"
    "\n"
    "  #ifdef MORPHTARGETS_TANGENT\n"
    "  tangentUpdated.xyz += (tangent{X} - tangent.xyz) * morphTargetInfluences[{X}];\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MORPH_TARGETS_VERTEX_FX_H
