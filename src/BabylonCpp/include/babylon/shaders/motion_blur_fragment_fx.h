#ifndef BABYLON_SHADERS_MOTION_BLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_MOTION_BLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* motionBlurPixelShader;

const char* motionBlurPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "\n"
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D velocitySampler;\n"
    "\n"
    "uniform float motionStrength;\n"
    "uniform float motionScale;\n"
    "uniform vec2 screenSize;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  #ifdef GEOMETRY_SUPPORTED\n"
    "  vec2 texelSize = 1.0 / screenSize;\n"
    "  vec2 velocityColor = texture2D(velocitySampler, vUV).rg * 2.0 - 1.0;\n"
    "  \n"
    "  vec2 velocity = vec2(pow(velocityColor.r, 3.0), pow(velocityColor.g, 3.0));\n"
    "  velocity *= motionScale * motionStrength;\n"
    "\n"
    "  float speed = length(velocity / texelSize);\n"
    "  int samplesCount = int(clamp(speed, 1.0, SAMPLES));\n"
    "\n"
    "  velocity = normalize(velocity) * texelSize;\n"
    "  float hlim = float(-samplesCount) * 0.5 + 0.5;\n"
    "\n"
    "  vec4 result = texture2D(textureSampler, vUV);\n"
    "\n"
    "  for (int i = 1; i < int(SAMPLES); ++i)\n"
    "  {\n"
    "  if (i >= samplesCount)\n"
    "  break;\n"
    "  \n"
    "  vec2 offset = vUV + velocity * (hlim + float(i));\n"
    "  result += texture2D(textureSampler, offset);\n"
    "  }\n"
    "\n"
    "  gl_FragColor = result / float(samplesCount);\n"
    "  gl_FragColor.a = 1.0;\n"
    "  #else\n"
    "  gl_FragColor = texture2D(textureSampler, vUV);\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_MOTION_BLUR_FRAGMENT_FX_H
