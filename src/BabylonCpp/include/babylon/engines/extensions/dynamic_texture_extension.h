#ifndef BABYLON_ENGINES_EXTENSIONS_DYNAMIC_TEXTURE_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_DYNAMIC_TEXTURE_EXTENSION_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

class ICanvas;
class InternalTexture;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT DynamicTextureExtension {

public:
  DynamicTextureExtension(ThinEngine* engine);
  ~DynamicTextureExtension();

  /**
   * @brief Creates a dynamic texture.
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param samplingMode defines the required sampling mode (Texture.NEAREST_SAMPLINGMODE by
   * default)
   * @returns the dynamic texture inside an InternalTexture
   */
  InternalTexturePtr createDynamicTexture(int width, int height, bool generateMipMaps,
                                          unsigned int samplingMode);

  /**
   * @brief Update the content of a dynamic texture.
   * @param texture defines the texture to update
   * @param canvas defines the canvas containing the source
   * @param invertY defines if data must be stored with Y axis inverted
   * @param premulAlpha defines if alpha is stored as premultiplied
   * @param format defines the format of the data
   * @param forceBindTexture if the texture should be forced to be bound eg. after a graphics
   * context loss (Default: false)
   */
  void updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* canvas, bool invertY,
                            bool premulAlpha                   = false,
                            std::optional<unsigned int> format = std::nullopt,
                            bool forceBindTexture              = false);

private:
  ThinEngine* _this;

}; // end of class DynamicTextureExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_DYNAMIC_TEXTURE_EXTENSION_H
