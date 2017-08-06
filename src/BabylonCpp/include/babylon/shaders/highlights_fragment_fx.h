#ifndef BABYLON_SHADERS_HIGHLIGHTS_FRAGMENT_FX_H
#define BABYLON_SHADERS_HIGHLIGHTS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* highlightsPixelShader;

const char* highlightsPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "const vec3 RGBLuminanceCoefficients = vec3(0.2126, 0.7152, 0.0722);\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  vec4 tex = texture2D(textureSampler, vUV);\n"
    "  vec3 c = tex.rgb;\n"
    "  float luma = dot(c.rgb, RGBLuminanceCoefficients);\n"
    "  \n"
    "  // to artificially desaturate/whiteout: c = mix(c, vec3(luma), luma * luma * luma * 0.1)\n"
    "  // brighter = higher luma = lower exponent = more diffuse glow\n"
    "  \n"
    "  gl_FragColor = vec4(pow(c, vec3(25.0 - luma * 15.0)), tex.a);  \n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_HIGHLIGHTS_FRAGMENT_FX_H
