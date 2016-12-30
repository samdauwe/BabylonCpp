#ifndef BABYLON_SHADERS_REFRACTION_FRAGMENT_FX_H
#define BABYLON_SHADERS_REFRACTION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* refractionPixelShader;

const char* refractionPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D refractionSampler;\n"
    "\n"
    "// Parameters\n"
    "uniform vec3 baseColor;\n"
    "uniform float depth;\n"
    "uniform float colorLevel;\n"
    "\n"
    "void main() {\n"
    "  float ref = 1.0 - texture2D(refractionSampler, vUV).r;\n"
    "\n"
    "  vec2 uv = vUV - vec2(0.5);\n"
    "  vec2 offset = uv * depth * ref;\n"
    "  vec3 sourceColor = texture2D(textureSampler, vUV - offset).rgb;\n"
    "\n"
    "  gl_FragColor = vec4(sourceColor + sourceColor * ref * colorLevel, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_REFRACTION_FRAGMENT_FX_H
