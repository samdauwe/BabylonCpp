#ifndef BABYLON_SHADERS_LAYER_FRAGMENT_FX_H
#define BABYLON_SHADERS_LAYER_FRAGMENT_FX_H

namespace BABYLON {

extern const char* layerPixelShader;

const char* layerPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "// Color\n"
    "uniform vec4 color;\n"
    "\n"
    "void main(void) {\n"
    "  vec4 baseColor = texture2D(textureSampler, vUV);\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "  if (baseColor.a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = baseColor * color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LAYER_FRAGMENT_FX_H
