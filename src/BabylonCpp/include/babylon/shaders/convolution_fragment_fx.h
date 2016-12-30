#ifndef BABYLON_SHADERS_CONVOLUTION_FRAGMENT_FX_H
#define BABYLON_SHADERS_CONVOLUTION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* convolutionPixelShader;

const char* convolutionPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "uniform vec2 screenSize;\n"
    "uniform float kernel[9];\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec2 onePixel = vec2(1.0, 1.0) / screenSize;\n"
    "  vec4 colorSum =\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(-1, -1)) * kernel[0] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(0, -1)) * kernel[1] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(1, -1)) * kernel[2] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(-1, 0)) * kernel[3] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(0, 0)) * kernel[4] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(1, 0)) * kernel[5] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(-1, 1)) * kernel[6] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(0, 1)) * kernel[7] +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(1, 1)) * kernel[8];\n"
    "\n"
    "  float kernelWeight =\n"
    "  kernel[0] +\n"
    "  kernel[1] +\n"
    "  kernel[2] +\n"
    "  kernel[3] +\n"
    "  kernel[4] +\n"
    "  kernel[5] +\n"
    "  kernel[6] +\n"
    "  kernel[7] +\n"
    "  kernel[8];\n"
    "\n"
    "  if (kernelWeight <= 0.0) {\n"
    "  kernelWeight = 1.0;\n"
    "  }\n"
    "\n"
    "  gl_FragColor = vec4((colorSum / kernelWeight).rgb, 1);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_CONVOLUTION_FRAGMENT_FX_H
