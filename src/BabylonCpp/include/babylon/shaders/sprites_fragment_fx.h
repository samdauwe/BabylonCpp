#ifndef BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H
#define BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* spritesPixelShader;

const char* spritesPixelShader
  = "uniform bool alphaTest;\n"
    "\n"
    "varying vec4 vColor;\n"
    "\n"
    "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "\n"
    "// Fog\n"
    "#include<fogFragmentDeclaration>\n"
    "\n"
    "void main(void) {\n"
    "  vec4 color = texture2D(diffuseSampler, vUV);\n"
    "\n"
    "  if (alphaTest) \n"
    "  {\n"
    "  if (color.a < 0.95)\n"
    "  discard;\n"
    "  }\n"
    "\n"
    "  color *= vColor;\n"
    "\n"
    "#include<fogFragment>\n"
    "\n"
    "  gl_FragColor = color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SPRITES_FRAGMENT_FX_H
