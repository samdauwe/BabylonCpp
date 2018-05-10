#ifndef BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* glowMapMergePixelShader;

const char* glowMapMergePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "#ifdef EMISSIVE\n"
    "  uniform sampler2D textureSampler2;\n"
    "#endif\n"
    "\n"
    "// Offset\n"
    "uniform float offset;\n"
    "\n"
    "void main(void) {\n"
    "  vec4 baseColor = texture2D(textureSampler, vUV);\n"
    "\n"
    "  #ifdef EMISSIVE\n"
    "  baseColor += texture2D(textureSampler2, vUV);\n"
    "  baseColor *= offset;\n"
    "  #else\n"
    "  baseColor.a = abs(offset - baseColor.a);\n"
    "\n"
    "  #ifdef STROKE\n"
    "  float alpha = smoothstep(.0, .1, baseColor.a);\n"
    "  baseColor.a = alpha;\n"
    "  baseColor.rgb = baseColor.rgb * alpha;\n"
    "  #endif\n"
    "  #endif\n"
    "\n"
    "  gl_FragColor = baseColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
