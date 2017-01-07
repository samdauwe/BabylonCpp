#ifndef BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H
#define BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* lensFlarePixelShader;

const char* lensFlarePixelShader
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
    "  gl_FragColor = baseColor * color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LENS_FLARE_FRAGMENT_FX_H
