#ifndef BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H

#include <functional>
#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

struct IMultiRenderTargetOptions;
struct ISize;
class ThinEngine;
FWD_CLASS_SPTR(InternalTexture)

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
   * @param initializeBuffers if set to true, the engine will make an initializing call of
   * drawBuffers
   * @returns the cube texture as an InternalTexture
   */
  std::vector<InternalTexturePtr>
  createMultipleRenderTarget(ISize size, const IMultiRenderTargetOptions& options,
                             bool initializeBuffers = true);

  /**
   * @brief Update the sample count for a given multiple render target texture.
   * @see https://doc.babylonjs.com/features/webgl2#multisample-render-targets
   * @param textures defines the textures to update
   * @param samples defines the sample count to set
   * @param initializeBuffers if set to true, the engine will make an initializing call of
   * drawBuffers
   * @returns the effective sample count (could be 0 if multisample render targets are not
   * supported)
   */
  unsigned int
  updateMultipleRenderTargetTextureSampleCount(const std::vector<InternalTexturePtr>& textures,
                                               unsigned int samples, bool initializeBuffers = true);

  /**
   * @brief Select a subsets of attachments to draw to.
   * @param attachments gl attachments
   */
  void bindAttachments(const std::vector<unsigned int>& attachments);

  /**
   * @brief Creates a layout object to draw/clear on specific textures in a MRT.
   * @param textureStatus textureStatus[i] indicates if the i-th is active
   * @returns A layout to be fed to the engine, calling `bindAttachments`.
   */
  std::vector<unsigned int> buildTextureLayout(const std::vector<bool>& textureStatus) const;

  /**
   * @brief Restores the webgl state to only draw on the main color attachment.
   * when the frame buffer associated is the canvas frame buffer
   */
  void restoreSingleAttachment();

  /**
   * @brief Restores the webgl state to only draw on the main color attachment.
   * when the frame buffer associated is not the canvas frame buffer
   */
  void restoreSingleAttachmentForRenderTarget();

  /**
   * @brief Clears a list of attachments.
   * @param attachments list of the attachments
   * @param colorMain clear color for the main attachment (the first one)
   * @param colorOthers clear color for the other attachments
   * @param clearDepth true to clear the depth buffer. Used only for the first attachment
   * @param clearStencil true to clear the stencil buffer. Used only for the first attachment
   */
  void clearAttachments(Uint32Array& attachments, const std::optional<Color4>& colorMain,
                        const std::optional<Color4>& colorOthers, bool clearDepth,
                        bool clearStencil);

private:
  ThinEngine* _this;

}; // end of class MultiRenderExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_MULTI_RENDER_EXTENSION_H
