#ifndef BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* outlinePixelShader;

const char* outlinePixelShader
  = "#ifdef LOGARITHMICDEPTH\n"
    "#extension GL_EXT_frag_depth : enable\n"
    "#endif\n"
    "uniform vec4 color;\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "#include<logDepthDeclaration>\n"
    "\n"
    "void main(void) {\n"
    "#ifdef ALPHATEST\n"
    "  if (texture2D(diffuseSampler, vUV).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "#include<logDepthFragment>\n"
    "  gl_FragColor = color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_OUTLINE_FRAGMENT_FX_H
