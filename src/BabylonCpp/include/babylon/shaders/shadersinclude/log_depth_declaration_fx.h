#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H

namespace BABYLON {

extern const char* logDepthDeclaration;

const char* logDepthDeclaration
  = "#ifdef LOGARITHMICDEPTH\n"
    "  uniform float logarithmicDepthConstant;\n"
    "  varying float vFragmentDepth;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LOG_DEPTH_DECLARATION_FX_H
