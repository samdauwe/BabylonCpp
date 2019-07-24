#ifndef BABYLON_MISC_TEXTURE_TOOLS_H
#define BABYLON_MISC_TEXTURE_TOOLS_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

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

}; // end of struct TextureTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_TEXTURE_TOOLS_H
