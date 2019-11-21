#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H

namespace BABYLON {

extern const char* logDepthDeclaration;

const char* logDepthDeclaration
  = R"ShaderCode(

#ifdef LOGARITHMICDEPTH
    uniform float logarithmicDepthConstant;
    varying float vFragmentDepth;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H
