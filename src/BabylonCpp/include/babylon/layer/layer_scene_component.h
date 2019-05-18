#ifndef BABYLON_LAYER_LAYER_SCENE_COMPONENT_H
#define BABYLON_LAYER_LAYER_SCENE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class Camera;
class Engine;
class LayerSceneComponent;
using LayerSceneComponentPtr = std::shared_ptr<LayerSceneComponent>;

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
    return std::shared_ptr<LayerSceneComponent>(
      new LayerSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~LayerSceneComponent();

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

private:
  void _draw(Camera* camera, bool isBackground);
  void _drawBackground(Camera* camera);
  void _drawForeground(Camera* camera);

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
