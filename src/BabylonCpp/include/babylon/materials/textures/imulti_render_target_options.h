#ifndef BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMultiRenderTargetOptions {
  nullable_t<bool> generateMipMaps        = nullopt_t;
  vector_t<unsigned int> types            = {};
  vector_t<unsigned int> samplingModes    = {};
  nullable_t<bool> generateDepthBuffer    = nullopt_t;
  nullable_t<bool> generateStencilBuffer  = nullopt_t;
  nullable_t<bool> generateDepthTexture   = nullopt_t;
  nullable_t<bool> doNotChangeAspectRatio = nullopt_t;
  nullable_t<size_t> textureCount         = nullopt_t;
  nullable_t<unsigned int> defaultType    = nullopt_t;
}; // end of struct IMultiRenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
