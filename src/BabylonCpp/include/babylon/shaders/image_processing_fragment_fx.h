#ifndef BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H
#define BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H

namespace BABYLON {

extern const char* imageProcessingPixelShader;

const char* imageProcessingPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "#include<imageProcessingDeclaration>\n"
    "\n"
    "#include<helperFunctions>\n"
    "\n"
    "#include<imageProcessingFunctions>\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 result = texture2D(textureSampler, vUV);\n"
    "\n"
    "#ifdef IMAGEPROCESSING\n"
    "  #ifndef FROMLINEARSPACE\n"
    "  // Need to move to linear space for subsequent operations.\n"
    "  result.rgb = toLinearSpace(result.rgb);\n"
    "  #endif\n"
    "\n"
    "  result = applyImageProcessing(result);\n"
    "#else\n"
    "  // In case where the input is in linear space we at least need to put it back in gamma.\n"
    "  #ifdef FROMLINEARSPACE\n"
    "  result = applyImageProcessing(result);\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = result;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_IMAGE_PROCESSING_FRAGMENT_FX_H
