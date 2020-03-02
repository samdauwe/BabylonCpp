#ifndef BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
#define BABYLON_RENDERING_UTILITY_LAYER_RENDER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Camera;
class HemisphericLight;
class PickingInfo;
class PointerInfo;
class PointerInfoPre;
class Scene;
using AbstractMeshPtr     = std::shared_ptr<AbstractMesh>;
using CameraPtr           = std::shared_ptr<Camera>;
using HemisphericLightPtr = std::shared_ptr<HemisphericLight>;

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
  static std::shared_ptr<UtilityLayerRenderer>& DefaultUtilityLayer();

  /**
   * @brief A shared utility layer that can be used to embed objects into a
   * scene (Depth map of the previous scene is not cleared before drawing on top
   * of it).
   */
  static std::shared_ptr<UtilityLayerRenderer>& DefaultKeepDepthUtilityLayer();

public:
  template <typename... Ts>
  static std::shared_ptr<UtilityLayerRenderer> New(Ts&&... args)
  {
    auto renderer = new UtilityLayerRenderer(std::forward<Ts>(args)...);
    return static_cast<std::shared_ptr<UtilityLayerRenderer>>(renderer);
  }
  ~UtilityLayerRenderer() override; // = default

  /**
   * @brief Gets the camera that is used to render the utility layer (when not
   * set, this will be the last active camera).
   * @returns the camera that is used when rendering the utility layer
   */
  CameraPtr getRenderCamera();

  /**
   * @brief Sets the camera that should be used when rendering the utility layer
   * (If set to null the last active camera will be used).
   * @param cam the camera that should be used when rendering the utility layer
   */
  void setRenderCamera(const CameraPtr& cam);

  /**
   * @brief Light which used by gizmos to get light shading.
   * Hidden
   */
  HemisphericLightPtr& _getSharedGizmoLight();

  /**
   * @brief Renders the utility layers scene on top of the original scene.
   */
  void render();

  /**
   * @brief Disposes of the renderer.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Instantiates a UtilityLayerRenderer.
   * @param originalScene the original scene that will be rendered on top of
   * @param handleEvents boolean indicating if the utility layer should handle
   * events
   */
  UtilityLayerRenderer(Scene* originalScene, bool handleEvents = true);

private:
  void _notifyObservers(const PointerInfoPre& prePointerInfo, const PickingInfo& pickInfo,
                        const PointerEvent& pointerEvent);
  void _updateCamera();

public:
  /**
   * If the picking should be done on the utility layer prior to the actual
   * scene (Default: true)
   */
  bool pickUtilitySceneFirst;

  /**
   * The scene that is rendered on top of the original scene
   */
  std::unique_ptr<Scene> utilityLayerScene;

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
   * Boolean indicating if the utility layer should handle events
   */
  bool handleEvents;

  /**
   * Observable raised when the pointer move from the utility layer scene to the
   * main scene
   */
  Observable<int> onPointerOutObservable;

  /**
   * Gets or sets a predicate that will be used to indicate utility meshes
   * present in the main scene
   */
  std::function<bool(const AbstractMeshPtr& mesh)> mainSceneTrackerPredicate;

private:
  static std::shared_ptr<UtilityLayerRenderer> _DefaultUtilityLayer;
  static std::shared_ptr<UtilityLayerRenderer> _DefaultKeepDepthUtilityLayer;

private:
  std::unordered_map<int, bool> _pointerCaptures;
  std::unordered_map<int, bool> _lastPointerEvents;
  HemisphericLightPtr _sharedGizmoLight;
  CameraPtr _renderCamera;
  Observer<Camera>::Ptr _afterRenderObserver;
  Observer<Scene>::Ptr _sceneDisposeObserver;
  Observer<PointerInfoPre>::Ptr _originalPointerObserver;

}; // end of class UtilityLayerRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_UTILITY_LAYER_RENDER_H
