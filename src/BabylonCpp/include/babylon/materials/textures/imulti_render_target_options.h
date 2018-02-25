#ifndef BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMultiRenderTargetOptions {
  Nullable<bool> generateMipMaps;
  vector_t<unsigned int> types         = {};
  vector_t<unsigned int> samplingModes = {};
  Nullable<bool> generateDepthBuffer;
  Nullable<bool> generateStencilBuffer;
  Nullable<bool> generateDepthTexture;
  Nullable<bool> doNotChangeAspectRatio;
  Nullable<size_t> textureCount;
  Nullable<unsigned int> defaultType;
}; // end of struct IMultiRenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
