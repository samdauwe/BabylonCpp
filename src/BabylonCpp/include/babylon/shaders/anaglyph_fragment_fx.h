#ifndef BABYLON_SHADERS_ANAGLYPH_FRAGMENT_FX_H
#define BABYLON_SHADERS_ANAGLYPH_FRAGMENT_FX_H

namespace BABYLON {

extern const char* anaglyphPixelShader;

const char* anaglyphPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D leftSampler;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 leftFrag = texture2D(leftSampler, vUV);\n"
    "  leftFrag = vec4(1.0, leftFrag.g, leftFrag.b, 1.0);\n"
    "\n"
    "  vec4 rightFrag = texture2D(textureSampler, vUV);\n"
    "  rightFrag = vec4(rightFrag.r, 1.0, 1.0, 1.0);\n"
    "\n"
    "  gl_FragColor = vec4(rightFrag.rgb * leftFrag.rgb, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_ANAGLYPH_FRAGMENT_FX_H
