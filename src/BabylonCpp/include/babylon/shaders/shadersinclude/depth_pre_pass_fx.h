#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H

namespace BABYLON {

extern const char* depthPrePass;

const char* depthPrePass
  = R"ShaderCode(

#ifdef DEPTHPREPASS
    gl_FragColor = vec4(0., 0., 0., 1.0);
    return;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_DEPTH_PRE_PASS_FX_H
