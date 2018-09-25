#ifndef BABYLON_TOOLS_TEXTURE_TOOLS_H
#define BABYLON_TOOLS_TEXTURE_TOOLS_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BaseTexture;
class Scene;
class Texture;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using TexturePtr     = std::shared_ptr<Texture>;

/**
 * @brief Represents the texture tools class.
 */
struct BABYLON_SHARED_EXPORT TextureTools {

  /**
   * @brief Uses the GPU to create a copy texture rescaled at a given size.
   * @param texture Texture to copy from
   * @param width Desired width
   * @param height Desired height
   * @return Generated texture
   */
  static TexturePtr CreateResizedCopy(const TexturePtr& texture, int width,
                                      int height, bool useBilinearMode = true);

  static BaseTexturePtr& GetEnvironmentBRDFTexture(Scene* scene);

  static std::string _environmentBRDFBase64Texture;

}; // end of struct TextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TEXTURE_TOOLS_H
