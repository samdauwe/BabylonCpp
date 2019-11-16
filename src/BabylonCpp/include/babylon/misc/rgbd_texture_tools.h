#ifndef BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
#define BABYLON_MISC_RGBD_TEXTURE_TOOLS_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/**
 * @brief Class used to host RGBD texture specific utilities.
 */
struct BABYLON_SHARED_EXPORT RGBDTextureTools {

  /**
   * @brief Expand the RGBD Texture from RGBD to Half Float if possible.
   * @param texture the texture to expand.
   */
  static void ExpandRGBDTexture(const TexturePtr& texture);

}; // end of struct RGBDTextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
