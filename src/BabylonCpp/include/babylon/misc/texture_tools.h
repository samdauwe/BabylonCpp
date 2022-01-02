#ifndef BABYLON_MISC_TEXTURE_TOOLS_H
#define BABYLON_MISC_TEXTURE_TOOLS_H

#include <memory>
#include <optional>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(Texture)

/**
 * @brief Class used to host texture specific utilities.
 */
struct BABYLON_SHARED_EXPORT TextureTools {

  /**
   * @brief Uses the GPU to create a copy texture rescaled at a given size.
   * @param texture Texture to copy from
   * @param width defines the desired width
   * @param height defines the desired height
   * @param useBilinearMode defines if bilinear mode has to be used
   * @return the generated texture
   */
  static TexturePtr CreateResizedCopy(const TexturePtr& texture, int width, int height,
                                      bool useBilinearMode = true);

  /**
   * @brief Apply a post process to a texture.
   * @param postProcessName name of the fragment post process
   * @param internalTexture the texture to encode
   * @param scene the scene hosting the texture
   * @param type type of the output texture. If not provided, use the one from internalTexture
   * @param samplingMode sampling mode to use to sample the source texture. If not provided, use the
   * one from internalTexture
   * @param format format of the output texture. If not provided, use the one from internalTexture
   * @return a promise with the internalTexture having its texture replaced by the result of the
   * processing
   */
  static InternalTexturePtr
  ApplyPostProcess(const std::string& postProcessName, const InternalTexturePtr& internalTexture,
                   Scene* scene, const std::optional<int>& type = std::nullopt,
                   const std::optional<unsigned int>& samplingMode = std::nullopt,
                   const std::optional<unsigned int>& format       = std::nullopt);

}; // end of struct TextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_TEXTURE_TOOLS_H
