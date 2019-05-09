#ifndef BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H

#include <babylon/shaders/shadersinclude/glsl_version_3.h>

namespace BABYLON {

extern const char* gpuUpdateParticlesPixelShader;

const char* gpuUpdateParticlesPixelShader
  = BABYLONCPP_GLSL_VERSION_3
    "\n"
    "void main() {\n"
    "  discard;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
