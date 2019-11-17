#ifndef BABYLON_LAYER_LAYER_SCENE_COMPONENT_H
#define BABYLON_LAYER_LAYER_SCENE_COMPONENT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class AbstractScene;
class Camera;
class Engine;
class Layer;
class LayerSceneComponent;
class RenderTargetTexture;
using LayerSceneComponentPtr = std::shared_ptr<LayerSceneComponent>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief Defines the layer scene component responsible to manage any layers
 * in a given scene.
 */
class BABYLON_SHARED_EXPORT LayerSceneComponent : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_LAYER;

public:
  template <typename... Ts>
  static LayerSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<LayerSceneComponent>(new LayerSceneComponent(std::forward<Ts>(args)...));
  }
  ~LayerSceneComponent() override; // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

  /**
   * @brief Adds all the elements from the container to the scene.
   * @param container the container holding the elements
   */
  void addFromContainer(const AbstractScene& container);

  /**
   * @brief Removes all the elements in the container from the scene.
   * @param container contains the elements to remove
   * @param dispose if the removed element should be disposed (default: false)
   */
  void removeFromContainer(const AbstractScene& container, bool dispose = false);

private:
  void _draw(const std::function<bool(const Layer& layer)>& predicate);
  bool _drawCameraPredicate(const Layer& layer, bool isBackground,
                            unsigned int cameraLayerMask) const;
  void _drawCameraBackground(Camera* camera);
  void _drawCameraForeground(Camera* camera);
  bool _drawRenderTargetPredicate(const Layer& layer, bool isBackground,
                                  unsigned int cameraLayerMask,
                                  const RenderTargetTexturePtr& renderTargetTexture) const;
  void _drawRenderTargetBackground(const RenderTargetTexturePtr& renderTarget);
  void _drawRenderTargetForeground(const RenderTargetTexturePtr& renderTarget);

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  LayerSceneComponent(Scene* scene);

private:
  Engine* _engine;

}; // end of class LayerSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_LAYER_SCENE_COMPONENT_H
