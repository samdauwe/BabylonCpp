#ifndef BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
#define BABYLON_SHADERS_COLOR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* colorPixelShader;

const char* colorPixelShader
  = "uniform vec4 color;\n"
    "\n"
    "void main(void) {\n"
    "  gl_FragColor = color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
