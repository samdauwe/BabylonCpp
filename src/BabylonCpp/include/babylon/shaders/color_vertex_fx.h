#ifndef BABYLON_SHADERS_COLOR_VERTEX_FX_H
#define BABYLON_SHADERS_COLOR_VERTEX_FX_H

namespace BABYLON {

extern const char* colorVertexShader;

const char* colorVertexShader
  = "// Attributes\n"
    "attribute vec3 position;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 worldViewProjection;\n"
    "\n"
    "void main(void) {\n"
    "  gl_Position = worldViewProjection * vec4(position, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_VERTEX_FX_H
