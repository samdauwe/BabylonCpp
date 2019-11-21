#ifndef BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H

#include <babylon/shaders/shadersinclude/glsl_version_3.h>

namespace BABYLON {

extern const char* gpuUpdateParticlesPixelShader;

const char* gpuUpdateParticlesPixelShader
  = R"ShaderCode(

BABYLONCPP_GLSL_VERSION_3

void main() {
  discard;
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
