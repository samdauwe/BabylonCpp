#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VERTEX_FX_H

namespace BABYLON {

extern const char* kernelBlurVertex;

const char* kernelBlurVertex
  = "sampleCoord{X} = sampleCenter + delta * KERNEL_OFFSET{X};\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_KERNEL_BLUR_VERTEX_FX_H
