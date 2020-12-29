#ifndef BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
#define BABYLON_MISC_RGBD_TEXTURE_TOOLS_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Engine;
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(Texture)

/**
 * @brief Class used to host RGBD texture specific utilities.
 */
struct BABYLON_SHARED_EXPORT RGBDTextureTools {

  /**
   * @brief Expand the RGBD Texture from RGBD to Half Float if possible.
   * @param texture the texture to expand.
   */
  static void ExpandRGBDTexture(const TexturePtr& texture);

  /**
   * @brief Expand the texture if possible by running the texture through the decode PP.
   * @param texture the texture to expand.
   */
  static void runRgbdDecodePostProcess(Texture* texture);

}; // end of struct RGBDTextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
