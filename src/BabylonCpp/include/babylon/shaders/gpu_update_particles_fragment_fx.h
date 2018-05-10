#ifndef BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* gpuUpdateParticlesPixelShader;

const char* gpuUpdateParticlesPixelShader
  = "#version 300 es\n"
    "\n"
    "void main() {\n"
    "  discard;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_UPDATE_PARTICLES_FRAGMENT_FX_H
