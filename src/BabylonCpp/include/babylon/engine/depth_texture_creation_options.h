#ifndef BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
#define BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Define options used to create a depth texture.
 */
struct BABYLON_SHARED_EXPORT DepthTextureCreationOptions {
  /** Specifies whether or not a stencil should be allocated in the texture */
  nullable_t<bool> generateStencil = nullopt_t;
  /** Specifies whether or not bilinear filtering is enable on the texture */
  nullable_t<bool> bilinearFiltering = nullopt_t;
  /** Specifies the comparison function to set on the texture. If 0 or
   * undefined, the texture is not in comparison mode */
  nullable_t<int> comparisonFunction = nullopt_t;
  /** Specifies if the created texture is a cube texture */
  nullable_t<bool> isCube = nullopt_t;
}; // end of struct DepthTextureCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_DEPTH_TEXTURE_CREATION_OPTIONS_H
