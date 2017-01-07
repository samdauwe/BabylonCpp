#ifndef BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* glowMapMergePixelShader;

const char* glowMapMergePixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "// Offset\n"
    "uniform float offset;\n"
    "\n"
    "void main(void) {\n"
    "  vec4 baseColor = texture2D(textureSampler, vUV);\n"
    "\n"
    "  baseColor.a = abs(offset - baseColor.a);\n"
    "\n"
    "  gl_FragColor = baseColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOW_MAP_MERGE_FRAGMENT_FX_H
