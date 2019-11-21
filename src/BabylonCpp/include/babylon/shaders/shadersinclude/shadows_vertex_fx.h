#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H

namespace BABYLON {

extern const char* shadowsVertex;

const char* shadowsVertex
  = R"ShaderCode(

#ifdef SHADOWS
    #if defined(SHADOW{X}) && !defined(SHADOWCUBE{X})
        vPositionFromLight{X} = lightMatrix{X} * worldPos;
        vDepthMetric{X} = ((vPositionFromLight{X}.z + light{X}.depthValues.x) / (light{X}.depthValues.y));
    #endif
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_VERTEX_FX_H
