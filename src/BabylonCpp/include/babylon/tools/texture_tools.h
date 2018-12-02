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
  static TexturePtr CreateResizedCopy(const TexturePtr& texture, int width,
                                      int height, bool useBilinearMode = true);

  /**
   * @brief Gets an environment BRDF texture for a given scene.
   * @param scene defines the hosting scene
   * @returns the environment BRDF texture
   */
  static BaseTexturePtr& GetEnvironmentBRDFTexture(Scene* scene);

  static std::string _environmentBRDFBase64Texture;

}; // end of struct TextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TEXTURE_TOOLS_H
