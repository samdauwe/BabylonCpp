#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H

namespace BABYLON {

extern const char* kernelBlurVaryingDeclaration;

const char* kernelBlurVaryingDeclaration
  = "varying vec2 sampleCoord{X};\n"
    "varying vec2 sampleCoord{X};\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H
