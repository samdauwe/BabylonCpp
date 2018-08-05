#ifndef BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
#define BABYLON_RENDERING_UTILITY_LAYER_RENDER_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Renders a layer on top of an existing scene.
 */
class BABYLON_SHARED_EXPORT UtilityLayerRenderer : public IDisposable {

public:
  /**
   * @brief A shared utility layer that can be used to overlay objects into a
   * scene (Depth map of the previous scene is cleared before drawing on top of
   * it).
   */
  static shared_ptr_t<UtilityLayerRenderer>& DefaultUtilityLayer();

  /**
   * @brief A shared utility layer that can be used to embed objects into a
   * scene (Depth map of the previous scene is not cleared before drawing on top
   * of it).
   */
  static shared_ptr_t<UtilityLayerRenderer>& DefaultKeepDepthUtilityLayer();

public:
  template <typename... Ts>
  static shared_ptr_t<UtilityLayerRenderer> New(Ts&&... args)
  {
    auto renderer = new UtilityLayerRenderer(::std::forward<Ts>(args)...);
    return static_cast<shared_ptr_t<UtilityLayerRenderer>>(renderer);
  }
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

protected:
  /**
   * @brief Instantiates a UtilityLayerRenderer.
   * @param originalScene the original scene that will be rendered on top of
   */
  UtilityLayerRenderer(Scene* originalScene);

private:
  void _notifyObservers(const PointerInfoPre& prePointerInfo,
                        const PickingInfo& pickInfo,
                        const PointerEvent& pointerEvent);
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
   * If set to true, only pointer down onPointerObservable events will be
   * blocked when picking is occluded by original scene
   */
  bool onlyCheckPointerDownEvents;

  /**
   * If set to false, only pointerUp, pointerDown and pointerMove will be sent
   * to the utilityLayerScene (false by default)
   */
  bool processAllEvents;

  /**
   * The original scene that will be rendered on top of
   */
  Scene* originalScene;

  /**
   * Observable raised when the pointer move from the utility layer scene to the
   * main scene
   */
  Observable<int> onPointerOutObservable;

  /**
   * Gets or sets a predicate that will be used to indicate utility meshes
   * present in the main scene
   */
  ::std::function<bool(AbstractMesh* mesh)> mainSceneTrackerPredicate;

private:
  static shared_ptr_t<UtilityLayerRenderer> _DefaultUtilityLayer;
  static shared_ptr_t<UtilityLayerRenderer> _DefaultKeepDepthUtilityLayer;

private:
  unordered_map_t<int, bool> _pointerCaptures;
  unordered_map_t<int, PointerType> _lastPointerEvents;
  Observer<Scene>::Ptr _afterRenderObserver;
  Observer<Scene>::Ptr _sceneDisposeObserver;
  Observer<PointerInfoPre>::Ptr _originalPointerObserver;

}; // end of class UtilityLayerRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
