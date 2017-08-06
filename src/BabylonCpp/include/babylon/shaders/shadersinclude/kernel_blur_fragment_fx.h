#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H

namespace BABYLON {

extern const char* kernelBlurFragment;

const char* kernelBlurFragment
  = "#ifdef PACKEDFLOAT\n"
    "  blend += unpack(texture2D(textureSampler, sampleCoord{X})) * KERNEL_WEIGHT{X};\n"
    "#else\n"
    "  blend += texture2D(textureSampler, sampleCoord{X}) * KERNEL_WEIGHT{X};\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_FRAGMENT_FX_H
