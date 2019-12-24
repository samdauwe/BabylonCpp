#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H

namespace BABYLON {

extern const char* kernelBlurVaryingDeclaration;

const char* kernelBlurVaryingDeclaration
  = R"ShaderCode(

varying vec2 sampleCoord{X};

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VARYING_DECLARATION_FX_H
