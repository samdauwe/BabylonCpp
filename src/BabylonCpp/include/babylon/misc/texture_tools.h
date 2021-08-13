#ifndef BABYLON_MISC_TEXTURE_TOOLS_H
#define BABYLON_MISC_TEXTURE_TOOLS_H

#include <memory>
#include <optional>
#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(InternalTexture)
FWD_CLASS_SPTR(Texture)

/**
 * @brief Class used to host texture specific utilities.
 */
class BABYLON_SHARED_EXPORT TextureTools {

public:
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

  // ref:
  // http://stackoverflow.com/questions/32633585/how-do-you-convert-to-half-floats-in-javascript
  /**
   * @brief Converts a number to half float.
   * @param value number to convert
   * @returns converted number
   */
  static int ToHalfFloat(float value);

  /**
   * @brief Converts a half float to a number.
   * @param value half float to convert
   * @returns converted half float
   */
  static int FromHalfFloat(int value);

private:
  static Float32Array _FloatView;
  static Int32Array _Int32View;

}; // end of class TextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_TEXTURE_TOOLS_H
