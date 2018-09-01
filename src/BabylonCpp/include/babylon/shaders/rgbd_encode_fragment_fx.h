#ifndef BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H
#define BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* rgbdEncodePixelShader;

const char* rgbdEncodePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "#include<helperFunctions>\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  gl_FragColor = toRGBD(texture2D(textureSampler, vUV).rgb);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_RGBD_ENCODE_FRAGMENT_FX_H
