#ifndef BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H
#define BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H

namespace BABYLON {

extern const char* vrMultiviewToSingleviewPixelShader;

const char* vrMultiviewToSingleviewPixelShader
  = "#ifdef GL_ES\n"
    "  precision mediump sampler2DArray;\n"
    "#endif\n"
    "\n"
    "varying vec2 vUV;\n"
    "uniform sampler2DArray multiviewSampler;\n"
    "uniform int imageIndex;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  gl_FragColor = texture(multiviewSampler, vec3(vUV, imageIndex));\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_VR_MULTIVIEW_TO_SINGLEVIEW_FRAGMENT_FX_H
