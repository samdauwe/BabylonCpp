#ifndef BABYLON_SHADERS_POSTPROCESS_VERTEX_FX_H
#define BABYLON_SHADERS_POSTPROCESS_VERTEX_FX_H

namespace BABYLON {

extern const char* postprocessVertexShader;

const char* postprocessVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "\n"
    "\n"
    "uniform vec2 scale;\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {  \n"
    "\n"
    "  vUV = (position * madd + madd) * scale;\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_POSTPROCESS_VERTEX_FX_H
