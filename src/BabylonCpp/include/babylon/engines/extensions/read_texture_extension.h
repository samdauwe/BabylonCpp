#ifndef BABYLON_ENGINES_EXTENSIONS_READ_TEXTURE_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_READ_TEXTURE_EXTENSION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/core/array_buffer_view.h>

namespace BABYLON {

class InternalTexture;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ReadTextureExtension {

public:
  ReadTextureExtension(ThinEngine* engine);
  ~ReadTextureExtension();

  /**
   * @brief Hidden
   */
  ArrayBufferView _readTexturePixels(const InternalTexturePtr& texture, int width, int height,
                                     int faceIndex = -1, int level = 0,
                                     std::optional<ArrayBufferView> buffer = std::nullopt);

private:
  ThinEngine* _this;

}; // end of class ReadTextureExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_READ_TEXTURE_EXTENSION_H
