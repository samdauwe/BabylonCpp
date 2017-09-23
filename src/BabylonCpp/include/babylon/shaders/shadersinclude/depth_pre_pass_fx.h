#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H

namespace BABYLON {

extern const char* depthPrePass;

const char* depthPrePass
  = "#ifdef DEPTHPREPASS\n"
    "  gl_FragColor = vec4(0., 0., 0., 1.0);\n"
    "  return;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H
