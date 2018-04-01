#ifndef BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
#define BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief Define options used to create a depth texture.
 */
struct BABYLON_SHARED_EXPORT DepthTextureCreationOptions {
  /** Specifies whether or not a stencil should be allocated in the texture */
  Nullable<bool> generateStencil = nullptr;
  /** Specifies whether or not bilinear filtering is enable on the texture */
  Nullable<bool> bilinearFiltering = nullptr;
  /** Specifies the comparison function to set on the texture. If 0 or
   * undefined, the texture is not in comparison mode */
  Nullable<int> comparisonFunction = nullptr;
  /** Specifies if the created texture is a cube texture */
  Nullable<bool> isCube = nullptr;
}; // end of struct DepthTextureCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
