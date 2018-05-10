#ifndef BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* gpuRenderParticlesPixelShader;

const char* gpuRenderParticlesPixelShader
  = "#version 300 es\n"
    "\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "in vec2 vUV;\n"
    "in vec4 vColor;\n"
    "\n"
    "out vec4 outFragColor;\n"
    "\n"
    "#ifdef CLIPPLANE\n"
    "in float fClipDistance;\n"
    "#endif\n"
    "\n"
    "void main() {\n"
    "#ifdef CLIPPLANE\n"
    "  if (fClipDistance > 0.0)\n"
    "  discard;\n"
    "#endif  \n"
    "  outFragColor = texture(textureSampler, vUV) * vColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H
