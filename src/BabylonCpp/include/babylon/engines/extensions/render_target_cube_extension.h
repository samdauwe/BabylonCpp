#ifndef BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_CUBE_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_CUBE_EXTENSION_H

#include <memory>
#include <variant>

#include <babylon/babylon_api.h>

namespace BABYLON {

class InternalTexture;
struct IRenderTargetOptions;
struct ISize;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT RenderTargetCubeExtension {

public:
  RenderTargetCubeExtension(ThinEngine* engine);
  ~RenderTargetCubeExtension();

  /**
   * @brief Creates a new render target cube texture.
   * @param size defines the size of the texture
   * @param options defines the options used to create the texture
   * @returns a new render target cube texture stored in an InternalTexture
   */
  InternalTexturePtr createRenderTargetCubeTexture(const ISize& size,
                                                   const IRenderTargetOptions& options);

private:
  ThinEngine* _this;

}; // end of class RenderTargetCubeExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_RENDER_TARGET_CUBE_EXTENSION_H
