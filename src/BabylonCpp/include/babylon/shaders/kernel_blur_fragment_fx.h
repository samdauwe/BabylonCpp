#ifndef BABYLON_SHADERS_KERNEL_BLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_KERNEL_BLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* kernelBlurPixelShader;

const char* kernelBlurPixelShader
  = "// Parameters\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec2 delta;\n"
    "\n"
    "// Varying\n"
    "varying vec2 sampleCenter;\n"
    "#include<kernelBlurVaryingDeclaration>[0..varyingCount]\n"
    "\n"
    "#ifdef PACKEDFLOAT\n"
    "  vec4 pack(float depth)\n"
    "  {\n"
    "  const vec4 bit_shift = vec4(255.0 * 255.0 * 255.0, 255.0 * 255.0, 255.0, 1.0);\n"
    "  const vec4 bit_mask = vec4(0.0, 1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0);\n"
    "\n"
    "  vec4 res = fract(depth * bit_shift);\n"
    "  res -= res.xxyz * bit_mask;\n"
    "\n"
    "  return res;\n"
    "  }\n"
    "\n"
    "  float unpack(vec4 color)\n"
    "  {\n"
    "  const vec4 bit_shift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);\n"
    "  return dot(color, bit_shift);\n"
    "  }\n"
    "#endif\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#ifdef PACKEDFLOAT  \n"
    "  float blend = 0.;\n"
    "#else\n"
    "  vec4 blend = vec4(0.);\n"
    "#endif\n"
    "\n"
    "  #include<kernelBlurFragment>[0..varyingCount]\n"
    "  #include<kernelBlurFragment2>[0..depCount]\n"
    "\n"
    "  #ifdef PACKEDFLOAT\n"
    "  gl_FragColor = pack(blend);\n"
    "  #else\n"
    "  gl_FragColor = blend;\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_KERNEL_BLUR_FRAGMENT_FX_H
