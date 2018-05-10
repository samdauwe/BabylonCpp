#ifndef BABYLON_SHADERS_GPU_RENDER_PARTICLES_VERTEX_FX_H
#define BABYLON_SHADERS_GPU_RENDER_PARTICLES_VERTEX_FX_H

namespace BABYLON {

extern const char* gpuRenderParticlesVertexShader;

const char* gpuRenderParticlesVertexShader
  = "#version 300 es\n"
    "\n"
    "uniform vec4 colorDead;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "// Particles state\n"
    "in vec3 position;\n"
    "in float age;\n"
    "in float life;\n"
    "in float size;\n"
    "in vec4 color;\n"
    "in vec2 offset;\n"
    "in vec2 uv;\n"
    "\n"
    "out vec2 vUV;\n"
    "out vec4 vColor;\n"
    "\n"
    "#ifdef CLIPPLANE\n"
    "uniform vec4 vClipPlane;\n"
    "uniform mat4 invView;\n"
    "out float fClipDistance;\n"
    "#endif\n"
    "\n"
    "\n"
    "void main() {\n"
    "  vUV = uv;\n"
    "  float ratio = age / life;\n"
    "  vColor = color * vec4(1.0 - ratio) + colorDead * vec4(ratio);\n"
    "\n"
    "  // Expand position\n"
    "  vec4 viewPosition = view * vec4(position, 1.0);\n"
    "  gl_Position = projection * (viewPosition + vec4(offset * size, 0., 0.));\n"
    "\n"
    "  // Clip plane\n"
    "#ifdef CLIPPLANE\n"
    "  vec4 worldPos = invView * viewPosition;\n"
    "  fClipDistance = dot(worldPos, vClipPlane);\n"
    "#endif  \n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_RENDER_PARTICLES_VERTEX_FX_H
