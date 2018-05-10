#ifndef BABYLON_SHADERS_SHARPEN_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHARPEN_FRAGMENT_FX_H

namespace BABYLON {

extern const char* sharpenPixelShader;

const char* sharpenPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec2 screenSize;\n"
    "uniform vec2 sharpnessAmounts;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec2 onePixel = vec2(1.0, 1.0) / screenSize;\n"
    "  vec4 color = texture2D(textureSampler, vUV);\n"
    "  vec4 edgeDetection = texture2D(textureSampler, vUV + onePixel * vec2(0, -1)) +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(-1, 0)) +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(1, 0)) +\n"
    "  texture2D(textureSampler, vUV + onePixel * vec2(0, 1)) -\n"
    "  color * 4.0;\n"
    "  \n"
    "  gl_FragColor = max(vec4(color.rgb * sharpnessAmounts.y, color.a) - (sharpnessAmounts.x * vec4(edgeDetection.rgb, 0)), 0.);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHARPEN_FRAGMENT_FX_H
