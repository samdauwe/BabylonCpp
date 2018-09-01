#ifndef BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_GPU_RENDER_PARTICLES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* gpuRenderParticlesPixelShader;

const char* gpuRenderParticlesPixelShader
  = "#version 300 es\n"
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
    "  outFragColor = texture(textureSampler, vUV) * vColor;\n"
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
