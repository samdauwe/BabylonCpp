#ifndef BABYLON_SHADERS_DISPLAYPASS_FRAGMENT_FX_H
#define BABYLON_SHADERS_DISPLAYPASS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* displayPassPixelShader;

const char* displayPassPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D passSampler;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  gl_FragColor = texture2D(passSampler, vUV);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DISPLAYPASS_FRAGMENT_FX_H
