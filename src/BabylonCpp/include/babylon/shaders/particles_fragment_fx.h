#ifndef BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H
#define BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H

namespace BABYLON {

extern const char* particlesPixelShader;

const char* particlesPixelShader
  = "// Samplers\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "uniform vec4 textureMask;\n"
    "uniform sampler2D diffuseSampler;\n"
    "\n"
    "#include<clipPlaneFragmentDeclaration>\n"
    "\n"
    "#include<imageProcessingDeclaration>\n"
    "\n"
    "#include<helperFunctions>\n"
    "\n"
    "#include<imageProcessingFunctions>\n"
    "\n"
    "void main(void) {\n"
    "  #include<clipPlaneFragment>\n"
    "\n"
    "  vec4 baseColor = texture2D(diffuseSampler, vUV);\n"
    "  baseColor = (baseColor * textureMask + (vec4(1., 1., 1., 1.) - textureMask)) * vColor;\n"
    "\n"
    "// Apply image processing if relevant. As this applies in linear space, \n"
    "// We first move from gamma to linear.\n"
    "#ifdef IMAGEPROCESSINGPOSTPROCESS\n"
    "  baseColor.rgb = toLinearSpace(baseColor.rgb);\n"
    "#else\n"
    "  #ifdef IMAGEPROCESSING\n"
    "  baseColor.rgb = toLinearSpace(baseColor.rgb);\n"
    "  baseColor = applyImageProcessing(baseColor);\n"
    "  #endif\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = baseColor;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PARTICLES_FRAGMENT_FX_H
