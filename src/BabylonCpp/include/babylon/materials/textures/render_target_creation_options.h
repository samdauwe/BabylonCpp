#ifndef BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_CREATION_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_CREATION_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Define options used to create a render target texture.
 */
struct BABYLON_SHARED_EXPORT RenderTargetCreationOptions {
  /**
   * Specifies if mipmaps must be created. If undefined, the value from generateMipMaps is taken
   * instead
   */
  std::optional<bool> createMipMaps = std::nullopt;
  /**
   * Specifies if mipmaps must be generated
   */
  std::optional<bool> generateMipMaps = std::nullopt;
  /**
   * Specifies whether or not a depth should be allocated in the texture (true
   * by default)
   */
  std::optional<bool> generateDepthBuffer = std::nullopt;
  /**
   * Specifies whether or not a stencil should be allocated in the texture
   * (false by default)
   */
  std::optional<bool> generateStencilBuffer = std::nullopt;
  /**
   * Defines texture type (int by default)
   */
  std::optional<unsigned int> type = std::nullopt;
  /**
   * Defines sampling mode (trilinear by default)
   */
  std::optional<unsigned int> samplingMode = std::nullopt;
  /**
   * Defines format (RGBA by default)
   */
  std::optional<unsigned int> format = std::nullopt;
  /**
   * Defines sample count (1 by default)
   */
  std::optional<unsigned int> samples = std::nullopt;
}; // end of struct RenderTargetCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RENDER_TARGET_CREATION_OPTIONS_H
