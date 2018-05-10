#ifndef BABYLON_SHADERS_GRAIN_FRAGMENT_FX_H
#define BABYLON_SHADERS_GRAIN_FRAGMENT_FX_H

namespace BABYLON {

extern const char* grainPixelShader;

const char* grainPixelShader
  = "#include<helperFunctions>\n"
    "\n"
    "// samplers\n"
    "uniform sampler2D textureSampler;  // original color\n"
    "\n"
    "// uniforms\n"
    "uniform float intensity;\n"
    "uniform float animatedSeed;\n"
    "\n"
    "// varyings\n"
    "varying vec2 vUV;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  gl_FragColor = texture2D(textureSampler, vUV);\n"
    "  vec2 seed = vUV*(animatedSeed);\n"
    "  float grain = dither(seed, intensity);\n"
    "\n"
    "  // Add less grain when luminance is high or low\n"
    "  float lum = getLuminance(gl_FragColor.rgb);\n"
    "  float grainAmount = (cos(-PI + (lum*PI*2.))+1.)/2.;\n"
    "  gl_FragColor.rgb += grain * grainAmount;\n"
    "\n"
    "  gl_FragColor.rgb = max(gl_FragColor.rgb, 0.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GRAIN_FRAGMENT_FX_H
