#ifndef BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMultiRenderTargetOptions {
  std::optional<bool> generateMipMaps        = std::nullopt;
  std::vector<unsigned int> types            = {};
  std::vector<unsigned int> samplingModes    = {};
  std::optional<bool> generateDepthBuffer    = std::nullopt;
  std::optional<bool> generateStencilBuffer  = std::nullopt;
  std::optional<bool> generateDepthTexture   = std::nullopt;
  std::optional<bool> doNotChangeAspectRatio = std::nullopt;
  std::optional<size_t> textureCount         = std::nullopt;
  std::optional<unsigned int> defaultType    = std::nullopt;
}; // end of struct IMultiRenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
