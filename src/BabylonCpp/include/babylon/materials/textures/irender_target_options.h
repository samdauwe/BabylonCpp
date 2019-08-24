#ifndef BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IRenderTargetOptions {
  std::optional<bool> generateMipMaps       = std::nullopt;
  std::optional<bool> generateDepthBuffer   = std::nullopt;
  std::optional<bool> generateStencilBuffer = std::nullopt;
  std::optional<unsigned int> type          = std::nullopt;
  std::optional<unsigned int> format        = std::nullopt;
  std::optional<unsigned int> samplingMode  = std::nullopt;
}; // end of struct RenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IRENDER_TARGET_OPTIONS_H
