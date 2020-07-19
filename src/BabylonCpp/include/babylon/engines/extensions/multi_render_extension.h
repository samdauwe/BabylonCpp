#ifndef BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H

#include <functional>
#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct IMultiRenderTargetOptions;
class InternalTexture;
struct ISize;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT MultiRenderExtension {

public:
  MultiRenderExtension(ThinEngine* engine);
  ~MultiRenderExtension();

  /**
   * @brief Unbind a list of render target textures from the webGL context.
   * This is used only when drawBuffer extension or webGL2 are active
   * @param textures defines the render target textures to unbind
   * @param disableGenerateMipMaps defines a boolean indicating that mipmaps must not be generated
   * @param onBeforeUnbind defines a function which will be called before the effective unbind
   */
  void unBindMultiColorAttachmentFramebuffer(const std::vector<InternalTexturePtr>& textures,
                                             bool disableGenerateMipMaps                 = false,
                                             const std::function<void()>& onBeforeUnbind = nullptr);

  /**
   * @brief Create a multi render target texture.
   * @see https://doc.babylonjs.com/features/webgl2#multiple-render-target
   * @param size defines the size of the texture
   * @param options defines the creation options
   * @returns the cube texture as an InternalTexture
   */
  std::vector<InternalTexturePtr>
  createMultipleRenderTarget(ISize size, const IMultiRenderTargetOptions& options);

  /**
   * @brief Update the sample count for a given multiple render target texture.
   * @see https://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param textures defines the textures to update
   * @param samples defines the sample count to set
   * @returns the effective sample count (could be 0 if multisample render targets are not
   * supported)
   */
  unsigned int
  updateMultipleRenderTargetTextureSampleCount(const std::vector<InternalTexturePtr>& textures,
                                               unsigned int samples);

  /**
   * @brief Select a subsets of attachments to draw to.
   * @param attachments gl attachments
   */
  void bindAttachments(const std::vector<unsigned int>& attachments);

private:
  ThinEngine* _this;

}; // end of class MultiRenderExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H
