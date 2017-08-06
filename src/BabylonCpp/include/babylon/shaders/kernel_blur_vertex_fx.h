#ifndef BABYLON_SHADERS_KERNEL_BLUR_VERTEX_FX_H
#define BABYLON_SHADERS_KERNEL_BLUR_VERTEX_FX_H

namespace BABYLON {

extern const char* kernelBlurVertexShader;

const char* kernelBlurVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "\n"
    "// Uniform\n"
    "uniform vec2 delta;\n"
    "\n"
    "// Output\n"
    "varying vec2 sampleCenter;\n"
    "#include<kernelBlurVaryingDeclaration>[0..varyingCount]\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {  \n"
    "\n"
    "  sampleCenter = (position * madd + madd);\n"
    "\n"
    "  #include<kernelBlurVertex>[0..varyingCount]\n"
    "\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_KERNEL_BLUR_VERTEX_FX_H
