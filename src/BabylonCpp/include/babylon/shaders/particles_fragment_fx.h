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
    "#ifdef RAMPGRADIENT\n"
    "varying vec4 remapRanges;\n"
    "uniform sampler2D rampSampler;\n"
    "#endif\n"
    "\n"
    "void main(void) {\n"
    "  #include<clipPlaneFragment>\n"
    "\n"
    "  vec4 textureColor = texture2D(diffuseSampler, vUV);\n"
    "  vec4 baseColor = (textureColor * textureMask + (vec4(1., 1., 1., 1.) - textureMask)) * vColor;\n"
    "\n"
    "  #ifdef RAMPGRADIENT\n"
    "  float alpha = baseColor.a;\n"
    "  float remappedColorIndex = clamp((alpha - remapRanges.x) / remapRanges.y, 0.0, 1.0);\n"
    "\n"
    "  vec4 rampColor = texture2D(rampSampler, vec2(1.0 - remappedColorIndex, 0.));\n"
    "  baseColor.rgb *= rampColor.rgb;\n"
    "\n"
    "  // Remapped alpha\n"
    "  float finalAlpha = baseColor.a;\n"
    "  baseColor.a = clamp((alpha * rampColor.a - remapRanges.z) / remapRanges.w, 0.0, 1.0);\n"
    "  #endif\n"
    "\n"
    "  #ifdef BLENDMULTIPLYMODE\n"
    "  float sourceAlpha = vColor.a * textureColor.a;\n"
    "  baseColor.rgb = baseColor.rgb * sourceAlpha + vec3(1.0) * (1.0 - sourceAlpha);\n"
    "  #endif\n"
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
