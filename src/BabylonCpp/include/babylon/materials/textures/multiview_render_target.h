#ifndef BABYLON_MATERIALS_TEXTURES_MULTIVIEW_RENDER_TARGET_H
#define BABYLON_MATERIALS_TEXTURES_MULTIVIEW_RENDER_TARGET_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

class MultiviewRenderTarget;
using MultiviewRenderTargetPtr = std::shared_ptr<MultiviewRenderTarget>;

/**
 * @brief Renders to multiple views with a single draw call.
 * @see https://www.khronos.org/registry/webgl/extensions/WEBGL_multiview/
 */
class BABYLON_SHARED_EXPORT MultiviewRenderTarget : public RenderTargetTexture {

public:
  template <typename... Ts>
  static MultiviewRenderTargetPtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<MultiviewRenderTarget>(
      new MultiviewRenderTarget(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~MultiviewRenderTarget();

  /**
   * @brief Binds a frame buffer.
   * @param faceIndex the face index, if its a cube texture
   */
  void _bindFrameBuffer(unsigned int faceIndex = 0);

  /**
   * @brief Gets the number of views the corresponding to the texture (eg. a
   * MultiviewRenderTarget will have > 1)
   * @returns the view count
   */
  unsigned int getViewCount() const;

protected:
  /**
   * @brief Creates a multiview render target.
   * @param scene scene used with the render target
   * @param size the size of the render target (used for each view)
   */
  MultiviewRenderTarget(Scene* scene, const ISize& size = ISize{512, 512});

}; // end of class MultiviewRenderTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MULTIVIEW_RENDER_TARGET_H
