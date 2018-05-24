#ifndef BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
#define BABYLON_RENDERING_UTILITY_LAYER_RENDER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Renders a layer on top of an existing scene.
 */
class BABYLON_SHARED_EXPORT UtilityLayerRenderer : public IDisposable {

public:
  /**
   * @brief Instantiates a UtilityLayerRenderer.
   * @param originalScene the original scene that will be rendered on top of
   */
  UtilityLayerRenderer(Scene* originalScene);
  virtual ~UtilityLayerRenderer();

  /**
   * @brief Renders the utility layers scene on top of the original scene.
   */
  void render();

  /**
   * @brief Disposes of the renderer.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

private:
  void _updateCamera();

public:
  /**
   * The scene that is rendered on top of the original scene
   */
  unique_ptr_t<Scene> utilityLayerScene;

  /**
   *  If the utility layer should automatically be rendered on top of existing
   * scene
   */
  bool shouldRender;

  /**
   * The original scene that will be rendered on top of
   */
  Scene* originalScene;

private:
  Observer<Scene>::Ptr _afterRenderObserver;
  Observer<Scene>::Ptr _sceneDisposeObserver;

}; // end of class UtilityLayerRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
