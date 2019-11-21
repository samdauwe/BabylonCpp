#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_VERTEX_FX_H

namespace BABYLON {

extern const char* logDepthVertex;

const char* logDepthVertex
  = R"ShaderCode(

#ifdef LOGARITHMICDEPTH
    vFragmentDepth = 1.0 + gl_Position.w;
    gl_Position.z = log2(max(0.000001, vFragmentDepth)) * logarithmicDepthConstant;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_VERTEX_FX_H
