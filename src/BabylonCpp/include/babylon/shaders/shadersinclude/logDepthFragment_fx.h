#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_LOGDEPTHFRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_LOGDEPTHFRAGMENT_FX_H

namespace BABYLON {

extern const char* logDepthFragment;

const char* logDepthFragment
  = "#ifdef LOGARITHMICDEPTH\n"
    "  gl_FragDepthEXT = log2(vFragmentDepth) * logarithmicDepthConstant * 0.5;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_LOGDEPTHFRAGMENT_FX_H
