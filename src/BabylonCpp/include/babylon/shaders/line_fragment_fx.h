#ifndef BABYLON_SHADERS_LINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_LINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* linePixelShader;

const char* linePixelShader
  = "uniform vec4 color;\n"
    "\n"
    "void main(void) {\n"
    "  gl_FragColor = color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_LINE_FRAGMENT_FX_H
