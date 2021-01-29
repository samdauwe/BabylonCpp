#ifndef BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
#define BABYLON_MISC_RGBD_TEXTURE_TOOLS_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

class Engine;
class Scene;
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

  /**
   * @brief Encode the texture to RGBD if possible.
   * @param internalTexture the texture to encode
   * @param scene the scene hosting the texture
   * @param outputTextureType type of the texture in which the encoding is performed
   * @return a promise with the internalTexture having its texture replaced by the result of the
   * processing
   */
  static InternalTexturePtr
  EncodeTextureToRGBD(const InternalTexturePtr& internalTexture, Scene* scene,
                      unsigned int outputTextureType = Constants::TEXTURETYPE_UNSIGNED_BYTE);

}; // end of struct RGBDTextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_RGBD_TEXTURE_TOOLS_H
