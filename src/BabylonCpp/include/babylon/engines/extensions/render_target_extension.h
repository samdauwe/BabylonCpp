#ifndef BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_EXTENSION_H

#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/meshes/abstract_mesh_constants.h>

namespace BABYLON {

struct DepthTextureCreationOptions;
class InternalTexture;
struct IRenderTargetOptions;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT RenderTargetExtension {

public:
  /**
   * Type used to define a render target texture size (either with a number or with a rect width and
   * height)
   */
  using RenderTargetTextureSize = std::variant<int, RenderTargetSize, float>;

public:
  RenderTargetExtension(ThinEngine* engine);
  ~RenderTargetExtension();

  /**
   * @brief Creates a new render target texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target texture stored in an InternalTexture
   */
  InternalTexturePtr createRenderTargetTexture(const RenderTargetTextureSize& size,
                                               const IRenderTargetOptions& options);

  /**
   * @brief Creates a depth stencil texture.
   * This is only available in WebGL 2 or with the depth texture extension available.
   * @param size The size of face edge in the texture.
   * @param options The options defining the texture.
   * @returns The texture
   */
  InternalTexturePtr createDepthStencilTexture(const RenderTargetTextureSize& size,
                                               const DepthTextureCreationOptions& options);

  /**
   * @brief Hidden
   */
  InternalTexturePtr _createDepthStencilTexture(const RenderTargetTextureSize& size,
                                                const DepthTextureCreationOptions& options);

private:
  ThinEngine* _this;

}; // end of class RenderTargetExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_EXTENSION_H
