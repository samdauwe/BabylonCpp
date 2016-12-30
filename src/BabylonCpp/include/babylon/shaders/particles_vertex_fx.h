#ifndef BABYLON_SHADERS_PARTICLES_VERTEX_FX_H
#define BABYLON_SHADERS_PARTICLES_VERTEX_FX_H

namespace BABYLON {

extern const char* particlesVertexShader;

const char* particlesVertexShader
  = "// Attributes\n"
    "attribute vec3 position;\n"
    "attribute vec4 color;\n"
    "attribute vec4 options;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "\n"
    "#ifdef CLIPPLANE\n"
    "uniform vec4 vClipPlane;\n"
    "uniform mat4 invView;\n"
    "varying float fClipDistance;\n"
    "#endif\n"
    "\n"
    "void main(void) {  \n"
    "  vec3 viewPos = (view * vec4(position, 1.0)).xyz; \n"
    "  vec3 cornerPos;\n"
    "  float size = options.y;\n"
    "  float angle = options.x;\n"
    "  vec2 offset = options.zw;\n"
    "\n"
    "  cornerPos = vec3(offset.x - 0.5, offset.y  - 0.5, 0.) * size;\n"
    "\n"
    "  // Rotate\n"
    "  vec3 rotatedCorner;\n"
    "  rotatedCorner.x = cornerPos.x * cos(angle) - cornerPos.y * sin(angle);\n"
    "  rotatedCorner.y = cornerPos.x * sin(angle) + cornerPos.y * cos(angle);\n"
    "  rotatedCorner.z = 0.;\n"
    "\n"
    "  // Position\n"
    "  viewPos += rotatedCorner;\n"
    "  gl_Position = projection * vec4(viewPos, 1.0);   \n"
    "  \n"
    "  vColor = color;\n"
    "  vUV = offset;\n"
    "\n"
    "  // Clip plane\n"
    "#ifdef CLIPPLANE\n"
    "  vec4 worldPos = invView * vec4(viewPos, 1.0);\n"
    "  fClipDistance = dot(worldPos, vClipPlane);\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PARTICLES_VERTEX_FX_H
