#ifndef BABYLON_SHADERS_DEPTHBOXBLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_DEPTHBOXBLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* depthBoxBlurPixelShader;

const char* depthBoxBlurPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "// Parameters\n"
    "uniform vec2 screenSize;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 colorDepth = vec4(0.0);\n"
    "\n"
    "  for (int x = -OFFSET; x <= OFFSET; x++)\n"
    "  for (int y = -OFFSET; y <= OFFSET; y++)\n"
    "  colorDepth += texture2D(textureSampler, vUV + vec2(x, y) / screenSize);\n"
    "\n"
    "  gl_FragColor = (colorDepth / float((OFFSET * 2 + 1) * (OFFSET * 2 + 1)));\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTHBOXBLUR_FRAGMENT_FX_H
