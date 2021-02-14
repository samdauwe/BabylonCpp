#ifndef BABYLON_MATERIALS_TEXTURES_PRE_PASS_RENDER_TARGET_H
#define BABYLON_MATERIALS_TEXTURES_PRE_PASS_RENDER_TARGET_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/textures/multi_render_target.h>

namespace BABYLON {

FWD_CLASS_SPTR(ImageProcessingPostProcess)

/**
 * @brief A multi render target designed to render the prepass.
 * Prepass is a scene component used to render information in multiple textures
 * alongside with the scene materials rendering.
 * Note : This is an internal class, and you should NOT need to instanciate this.
 * Only the `PrePassRenderer` should instanciate this class.
 * It is more likely that you need a regular `MultiRenderTarget`
 * @hidden
 */
class BABYLON_SHARED_EXPORT PrePassRenderTarget : public MultiRenderTarget {

public:
  PrePassRenderTarget(const std::string& name, const RenderTargetTexturePtr& renderTargetTexture,
                      const std::variant<int, RenderTargetSize, float>& size, size_t count,
                      Scene* scene,
                      const std::optional<IMultiRenderTargetOptions>& options = std::nullopt);
  ~PrePassRenderTarget() override; // = default

  /**
   * @brief Creates a composition effect for this RT.
   * @hidden
   */
  void _createCompositionEffect();

  /**
   * @brief Checks that the size of this RT is still adapted to the desired render size.
   * @hidden
   */
  void _checkSize();

  /**
   * @brief Changes the number of render targets in this MRT.
   * Be careful as it will recreate all the data in the new texture.
   * @param count new texture count
   * @param options Specifies texture types and sampling modes for new textures
   */
  void updateCount(size_t count,
                   const std::optional<IMultiRenderTargetOptions>& options = std::nullopt) override;

  /**
   * @brief Resets the post processes chains applied to this RT.
   * @hidden
   */
  void _resetPostProcessChain();

  /**
   * @brief Diposes this render target.
   */
  void dispose() override;

public:
  /**
   * @hidden
   */
  std::vector<PostProcessPtr> _beforeCompositionPostProcesses;

  /**
   * Image processing post process for composition
   */
  ImageProcessingPostProcessPtr imageProcessingPostProcess;

  /**
   * @hidden
   */
  Engine* _engine;

  /**
   * @hidden
   */
  Scene* _scene;

  /**
   * @hidden
   */
  PostProcessPtr _outputPostProcess;

  /**
   * @hidden
   */
  bool _internalTextureDirty;

  /**
   * Is this render target enabled for prepass rendering
   */
  bool enabled;

  /**
   * Render target associated with this prePassRenderTarget
   * If this is `null`, it means this prePassRenderTarget is associated with the scene
   */
  RenderTargetTexturePtr renderTargetTexture;

}; // end of class PrePassRenderTarget

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_PRE_PASS_RENDER_TARGET_H
