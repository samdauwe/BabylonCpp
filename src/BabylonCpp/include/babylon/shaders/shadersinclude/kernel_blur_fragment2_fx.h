#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H

namespace BABYLON {

extern const char* kernelBlurFragment2;

const char* kernelBlurFragment2
  = "#ifdef PACKEDFLOAT\n"
    "  blend += unpack(texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X})) * KERNEL_DEP_WEIGHT{X};\n"
    "#else\n"
    "  blend += texture2D(textureSampler, sampleCenter + delta * KERNEL_DEP_OFFSET{X}) * KERNEL_DEP_WEIGHT{X};\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT2_FX_H
