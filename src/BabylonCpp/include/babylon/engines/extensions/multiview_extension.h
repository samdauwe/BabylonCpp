#ifndef BABYLON_ENGINES_EXTENSIONS_MULTIVIEW_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_MULTIVIEW_EXTENSION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Engine;
FWD_CLASS_SPTR(nternalTexture)

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT MultiviewExtension {

public:
  MultiviewExtension(Engine* engine);
  ~MultiviewExtension();

  /**
   * @brief Creates a new multiview render target.
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @returns the created multiview texture
   */
  InternalTexturePtr createMultiviewRenderTargetTexture(int width, int height);

  /**
   * @brief Binds a multiview framebuffer to be drawn to.
   * @param multiviewTexture texture to bind
   */
  void bindMultiviewFramebuffer(const InternalTexturePtr& multiviewTexture);

private:
  Engine* _this;

}; // end of class MultiviewExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_MULTIVIEW_EXTENSION_H
