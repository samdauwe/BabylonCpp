#ifndef BABYLON_SHADERS_EXTRACT_HIGHLIGHTS_FRAGMENT_FX_H
#define BABYLON_SHADERS_EXTRACT_HIGHLIGHTS_FRAGMENT_FX_H

namespace BABYLON {

extern const char* extractHighlightsPixelShader;

const char* extractHighlightsPixelShader
  = "#include<helperFunctions>\n"
    "\n"
    "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform float threshold;\n"
    "uniform float exposure;\n"
    "\n"
    "void main(void) \n"
    "{\n"
    "  gl_FragColor = texture2D(textureSampler, vUV);\n"
    "  float luma = getLuminance(gl_FragColor.rgb * exposure);\n"
    "  gl_FragColor.rgb = step(threshold, luma) * gl_FragColor.rgb;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_EXTRACT_HIGHLIGHTS_FRAGMENT_FX_H
