#ifndef BABYLON_SHADERS_GLOW_MAP_GENERATION_FRAGMENT_FX_H
#define BABYLON_SHADERS_GLOW_MAP_GENERATION_FRAGMENT_FX_H

namespace BABYLON {

extern const char* glowMapGenerationPixelShader;

const char* glowMapGenerationPixelShader
  = "#ifdef ALPHATEST\n"
    "varying vec2 vUVDiffuse;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "#ifdef EMISSIVE\n"
    "varying vec2 vUVEmissive;\n"
    "uniform sampler2D emissiveSampler;\n"
    "#endif\n"
    "\n"
    "uniform vec4 color;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#ifdef ALPHATEST\n"
    "  if (texture2D(diffuseSampler, vUVDiffuse).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "#ifdef EMISSIVE\n"
    "  gl_FragColor = texture2D(emissiveSampler, vUVEmissive) * color;\n"
    "#else\n"
    "  gl_FragColor = color;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOW_MAP_GENERATION_FRAGMENT_FX_H
