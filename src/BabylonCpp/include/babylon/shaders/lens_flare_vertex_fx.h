#ifndef BABYLON_SHADERS_LENS_FLARE_VERTEX_FX_H
#define BABYLON_SHADERS_LENS_FLARE_VERTEX_FX_H

namespace BABYLON {

extern const char* lensFlareVertexShader;

const char* lensFlareVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 viewportMatrix;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {  \n"
    "\n"
    "  vUV = position * madd + madd;\n"
    "  gl_Position = viewportMatrix * vec4(position, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LENS_FLARE_VERTEX_FX_H
