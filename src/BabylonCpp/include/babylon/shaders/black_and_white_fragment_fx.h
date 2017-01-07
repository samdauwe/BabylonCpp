#ifndef BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
#define BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* blackAndWhitePixelShader;

const char* blackAndWhitePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  float luminance = dot(texture2D(textureSampler, vUV).rgb, vec3(0.3, 0.59, 0.11));\n"
    "  gl_FragColor = vec4(luminance, luminance, luminance, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
