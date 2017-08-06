#ifndef BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
#define BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* blackAndWhitePixelShader;

const char* blackAndWhitePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform float degree;\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  vec3 color = texture2D(textureSampler, vUV).rgb;\n"
    "  float luminance = dot(color, vec3(0.3, 0.59, 0.11));    \n"
    "  vec3 blackAndWhite = vec3(luminance, luminance, luminance);\n"
    "  gl_FragColor = vec4(color - ((color - blackAndWhite) * degree), 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_BLACK_AND_WHITE_FRAGMENT_FX_H
