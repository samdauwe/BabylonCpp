#ifndef BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H
#define BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* rgbdDecodePixelShader;

const char* rgbdDecodePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "#include<helperFunctions>\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  gl_FragColor = vec4(fromRGBD(texture2D(textureSampler, vUV)), 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_RGBD_DECODE_FRAGMENT_FX_H
