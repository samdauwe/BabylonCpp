#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* kernelBlurFragment;

const char* kernelBlurFragment
  = R"ShaderCode(

#ifdef DOF
    factor = sampleCoC(sampleCoord{X});
    computedWeight = KERNEL_WEIGHT{X} * factor;
    sumOfWeights += computedWeight;
#else
    computedWeight = KERNEL_WEIGHT{X};
#endif

#ifdef PACKEDFLOAT
    blend += unpack(texture2D(textureSampler, sampleCoord{X})) * computedWeight;
#else
    blend += texture2D(textureSampler, sampleCoord{X}) * computedWeight;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H
