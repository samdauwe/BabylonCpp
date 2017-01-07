#ifndef BABYLON_SHADERS_STEREOSCOPIC_INTERLACE_FRAGMENT_FX_H
#define BABYLON_SHADERS_STEREOSCOPIC_INTERLACE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* stereoscopicInterlacePixelShader;

const char* stereoscopicInterlacePixelShader
  = "const vec3 TWO = vec3(2.0, 2.0, 2.0);\n"
    "\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D camASampler;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec2 stepSize;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  bool useCamB;\n"
    "  vec2 texCoord1;\n"
    "  vec2 texCoord2;\n"
    "  \n"
    "  vec3 frag1;\n"
    "  vec3 frag2;\n"
    "  \n"
    "#ifdef IS_STEREOSCOPIC_HORIZ\n"
    "  useCamB = vUV.x > 0.5;\n"
    "  texCoord1 = vec2(useCamB ? (vUV.x - 0.5) * 2.0 : vUV.x * 2.0, vUV.y);\n"
    "  texCoord2 = vec2(texCoord1.x + stepSize.x, vUV.y);\n"
    "#else\n"
    "  useCamB = vUV.y > 0.5;\n"
    "  texCoord1 = vec2(vUV.x, useCamB ? (vUV.y - 0.5) * 2.0 : vUV.y * 2.0);\n"
    "  texCoord2 = vec2(vUV.x, texCoord1.y + stepSize.y);\n"
    "#endif\n"
    "  \n"
    "  // cannot assign a sampler to a variable, so must duplicate texture accesses\n"
    "  if (useCamB){\n"
    "  frag1 = texture2D(textureSampler, texCoord1).rgb;\n"
    "  frag2 = texture2D(textureSampler, texCoord2).rgb;\n"
    "  }else{\n"
    "  frag1 = texture2D(camASampler   , texCoord1).rgb;\n"
    "  frag2 = texture2D(camASampler   , texCoord2).rgb;\n"
    "  }\n"
    "  \n"
    "  gl_FragColor = vec4((frag1 + frag2) / TWO, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_STEREOSCOPIC_INTERLACE_FRAGMENT_FX_H
