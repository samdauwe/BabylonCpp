#ifndef BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* bloomMergePixelShader;

const char* bloomMergePixelShader
  = "uniform sampler2D textureSampler;\n"
    "uniform sampler2D bloomBlur;\n"
    "\n"
    "varying vec2 vUV;\n"
    "uniform float bloomWeight;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  gl_FragColor = texture2D(textureSampler, vUV);\n"
    "  vec3 blurred = texture2D(bloomBlur, vUV).rgb;\n"
    "  gl_FragColor.rgb = gl_FragColor.rgb + (blurred.rgb * bloomWeight); \n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_BLOOM_MERGE_FRAGMENT_FX_H
