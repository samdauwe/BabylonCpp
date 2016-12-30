#ifndef BABYLON_SHADERS_LAYER_VERTEX_FX_H
#define BABYLON_SHADERS_LAYER_VERTEX_FX_H

namespace BABYLON {

extern const char* layerVertexShader;

const char* layerVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "\n"
    "// Uniforms\n"
    "uniform vec2 scale;\n"
    "uniform vec2 offset;\n"
    "uniform mat4 textureMatrix;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {  \n"
    "  vec2 shiftedPosition = position * scale + offset;\n"
    "  vUV = vec2(textureMatrix * vec4(shiftedPosition * madd + madd, 1.0, 0.0));\n"
    "  gl_Position = vec4(shiftedPosition, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LAYER_VERTEX_FX_H
