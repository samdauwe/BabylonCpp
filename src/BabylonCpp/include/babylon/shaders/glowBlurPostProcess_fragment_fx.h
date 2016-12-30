#ifndef BABYLON_SHADERS_GLOWBLURPOSTPROCESS_FRAGMENT_FX_H
#define BABYLON_SHADERS_GLOWBLURPOSTPROCESS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* glowBlurPostProcessPixelShader;

const char* glowBlurPostProcessPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "// Parameters\n"
    "uniform vec2 screenSize;\n"
    "uniform vec2 direction;\n"
    "uniform float blurWidth;\n"
    "\n"
    "// Transform color to luminance.\n"
    "float getLuminance(vec3 color)\n"
    "{\n"
    "  return dot(color, vec3(0.2126, 0.7152, 0.0722));\n"
    "}\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  float weights[7];\n"
    "  weights[0] = 0.05;\n"
    "  weights[1] = 0.1;\n"
    "  weights[2] = 0.2;\n"
    "  weights[3] = 0.3;\n"
    "  weights[4] = 0.2;\n"
    "  weights[5] = 0.1;\n"
    "  weights[6] = 0.05;\n"
    "\n"
    "  vec2 texelSize = vec2(1.0 / screenSize.x, 1.0 / screenSize.y);\n"
    "  vec2 texelStep = texelSize * direction * blurWidth;\n"
    "  vec2 start = vUV - 3.0 * texelStep;\n"
    "\n"
    "  vec4 baseColor = vec4(0., 0., 0., 0.);\n"
    "  vec2 texelOffset = vec2(0., 0.);\n"
    "\n"
    "  for (int i = 0; i < 7; i++)\n"
    "  {\n"
    "  // alpha blur.\n"
    "  vec4 texel = texture2D(textureSampler, start + texelOffset);\n"
    "  baseColor.a += texel.a * weights[i];\n"
    "\n"
    "  // Highest Luma for outline.\n"
    "  float luminance = getLuminance(baseColor.rgb);\n"
    "  float luminanceTexel = getLuminance(texel.rgb);\n"
    "  float choice = step(luminanceTexel, luminance);\n"
    "  baseColor.rgb = choice * baseColor.rgb + (1.0 - choice) * texel.rgb;\n"
    "\n"
    "  texelOffset += texelStep;\n"
    "  }\n"
    "\n"
    "  gl_FragColor = baseColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOWBLURPOSTPROCESS_FRAGMENT_FX_H
