#ifndef BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMultiRenderTargetOptions {
  bool generateMipMaps                 = false;
  vector_t<unsigned int> types         = {};
  vector_t<unsigned int> samplingModes = {};
  bool generateDepthBuffer             = true;
  bool generateStencilBuffer           = false;
  bool generateDepthTexture            = false;
  bool doNotChangeAspectRatio          = true;
  std::size_t textureCount             = 0;
}; // end of struct IMultiRenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
