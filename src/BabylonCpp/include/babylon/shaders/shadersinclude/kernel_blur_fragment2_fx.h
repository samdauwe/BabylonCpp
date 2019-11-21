#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H

namespace BABYLON {

extern const char* kernelBlurFragment2;

const char* kernelBlurFragment2
  = R"ShaderCode(

#ifdef DOF
    factor = sampleCoC(sampleCenter + delta * KERNEL_DEP_OFFSET{X});
    computedWeight = KERNEL_DEP_WEIGHT{X} * factor;
    sumOfWeights += computedWeight;
#else
    computedWeight = KERNEL_DEP_WEIGHT{X};
#endif

#ifdef PACKEDFLOAT
    blend += unpack(texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X})) * computedWeight;
#else
    blend += texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X}) * computedWeight;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
