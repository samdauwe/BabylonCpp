#ifndef BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
#define BABYLON_SHADERS_COLOR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* colorPixelShader;

const char* colorPixelShader
  = "\n"
    "#ifdef VERTEXCOLOR\n"
    "varying vec4 vColor;\n"
    "#else\n"
    "uniform vec4 color;\n"
    "#endif\n"
    "\n"
    "void main(void) {\n"
    "#ifdef VERTEXCOLOR\n"
    "  gl_FragColor = vColor;\n"
    "#else\n"
    "  gl_FragColor = color;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_COLOR_FRAGMENT_FX_H
