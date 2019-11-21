#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_FRAGMENT_FX_H

namespace BABYLON {

extern const char* logDepthFragment;

const char* logDepthFragment
  = R"ShaderCode(

#ifdef LOGARITHMICDEPTH
    gl_FragDepthEXT = log2(vFragmentDepth) * logarithmicDepthConstant * 0.5;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_FRAGMENT_FX_H
