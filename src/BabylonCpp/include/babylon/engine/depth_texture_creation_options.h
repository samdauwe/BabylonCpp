#ifndef BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
#define BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Define options used to create a depth texture.
 */
struct BABYLON_SHARED_EXPORT DepthTextureCreationOptions {
  /** Specifies whether or not a stencil should be allocated in the texture */
  std::optional<bool> generateStencil = std::nullopt;
  /** Specifies whether or not bilinear filtering is enable on the texture */
  std::optional<bool> bilinearFiltering = std::nullopt;
  /** Specifies the comparison function to set on the texture. If 0 or
   * undefined, the texture is not in comparison mode */
  std::optional<int> comparisonFunction = std::nullopt;
  /** Specifies if the created texture is a cube texture */
  std::optional<bool> isCube = std::nullopt;
}; // end of struct DepthTextureCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
