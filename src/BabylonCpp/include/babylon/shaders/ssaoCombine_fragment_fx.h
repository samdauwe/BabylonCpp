#ifndef BABYLON_SHADERS_SSAOCOMBINE_FRAGMENT_FX_H
#define BABYLON_SHADERS_SSAOCOMBINE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* ssaoCombinePixelShader;

const char* ssaoCombinePixelShader
  = "uniform sampler2D textureSampler;\n"
    "uniform sampler2D originalColor;\n"
    "\n"
    "varying vec2 vUV;\n"
    "\n"
    "void main(void) {\n"
    "  vec4 ssaoColor = texture2D(textureSampler, vUV);\n"
    "  vec4 sceneColor = texture2D(originalColor, vUV);\n"
    "\n"
    "  gl_FragColor = sceneColor * ssaoColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SSAOCOMBINE_FRAGMENT_FX_H
