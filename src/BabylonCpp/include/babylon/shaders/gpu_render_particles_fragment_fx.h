#ifndef BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H

#include <babylon/shaders/shadersinclude/glsl_version_3.h>

namespace BABYLON {

extern const char* gpuRenderParticlesPixelShader;

const char* gpuRenderParticlesPixelShader
  = BABYLONCPP_GLSL_VERSION_3
    "\n"
    "uniform sampler2D textureSampler;\n"
    "\n"
    "in vec2 vUV;\n"
    "in vec4 vColor;\n"
    "\n"
    "out vec4 outFragColor;\n"
    "\n"
    "#include<clipPlaneFragmentDeclaration2> \n"
    "\n"
    "#include<imageProcessingDeclaration>\n"
    "\n"
    "#include<helperFunctions>\n"
    "\n"
    "#include<imageProcessingFunctions>\n"
    "\n"
    "void main() {\n"
    "  #include<clipPlaneFragment> \n"
    "  vec4 textureColor = texture(textureSampler, vUV);\n"
    "  outFragColor = textureColor * vColor;\n"
    "\n"
    "  #ifdef BLENDMULTIPLYMODE\n"
    "  float alpha = vColor.a * textureColor.a;\n"
    "  outFragColor.rgb = outFragColor.rgb * alpha + vec3(1.0) * (1.0 - alpha);  \n"
    "  #endif    \n"
    "\n"
    "// Apply image processing if relevant. As this applies in linear space, \n"
    "// We first move from gamma to linear.\n"
    "#ifdef IMAGEPROCESSINGPOSTPROCESS\n"
    "  outFragColor.rgb = toLinearSpace(outFragColor.rgb);\n"
    "#else\n"
    "  #ifdef IMAGEPROCESSING\n"
    "  outFragColor.rgb = toLinearSpace(outFragColor.rgb);\n"
    "  outFragColor = applyImageProcessing(outFragColor);\n"
    "  #endif\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H
