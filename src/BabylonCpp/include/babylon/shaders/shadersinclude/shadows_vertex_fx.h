#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H

namespace BABYLON {

extern const char* shadowsVertex;

const char* shadowsVertex
  = "#ifdef SHADOWS\n"
    "  #if defined(SHADOW{X}) && !defined(SHADOWCUBE{X})\n"
    "  vPositionFromLight{X} = lightMatrix{X} * worldPos;\n"
    "  vDepthMetric{X} =  ((vPositionFromLight{X}.z + light{X}.depthValues.x) / (light{X}.depthValues.y));\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H
