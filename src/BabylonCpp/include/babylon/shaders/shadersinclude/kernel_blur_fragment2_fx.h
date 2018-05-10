#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H

namespace BABYLON {

extern const char* kernelBlurFragment2;

const char* kernelBlurFragment2
  = "#ifdef DOF\n"
    "  factor = sampleCoC(sampleCenter + delta * KERNEL_DEP_OFFSET{X});\n"
    "  computedWeight = KERNEL_DEP_WEIGHT{X} * factor;\n"
    "  sumOfWeights += computedWeight;\n"
    "#else\n"
    "  computedWeight = KERNEL_DEP_WEIGHT{X};\n"
    "#endif\n"
    "\n"
    "#ifdef PACKEDFLOAT\n"
    "  blend += unpack(texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X})) * computedWeight;\n"
    "#else\n"
    "  blend += texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X}) * computedWeight;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
