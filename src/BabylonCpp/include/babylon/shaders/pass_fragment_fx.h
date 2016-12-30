#ifndef BABYLON_SHADERS_PASS_FRAGMENT_FX_H
#define BABYLON_SHADERS_PASS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* passPixelShader;

const char* passPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  gl_FragColor = texture2D(textureSampler, vUV);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PASS_FRAGMENT_FX_H
