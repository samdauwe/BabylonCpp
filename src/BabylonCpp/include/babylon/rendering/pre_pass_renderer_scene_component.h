#ifndef BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H
#define BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/iscene_serializable_component.h>
#include <babylon/engines/scene_component_constants.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractScene;
class PrePassRendererSceneComponent;
using PrePassRendererSceneComponentPtr = std::shared_ptr<PrePassRendererSceneComponent>;

/**
 * @brief Defines the Geometry Buffer scene component responsible to manage a G-Buffer useful
 * in several rendering techniques.
 */
class BABYLON_SHARED_EXPORT PrePassRendererSceneComponent : public ISceneSerializableComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_PREPASSRENDERER;

public:
  template <typename... Ts>
  static PrePassRendererSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<PrePassRendererSceneComponent>(
      new PrePassRendererSceneComponent(std::forward<Ts>(args)...));
  }
  ~PrePassRendererSceneComponent() override; // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Serializes the component data to the specified json object.
   * @param serializationObject The object to serialize to
   */
  void serialize(json& serializationObject) override;

  /**
   * @brief Adds all the elements from the container to the scene.
   * @param container the container holding the elements
   */
  void addFromContainer(AbstractScene& container) override;

  /**
   * @brief Removes all the elements in the container from the scene.
   * @param container contains the elements to remove
   * @param dispose if the removed element should be disposed (default: false)
   */
  void removeFromContainer(AbstractScene& container, bool dispose = false) override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  PrePassRendererSceneComponent(Scene* scene);

private:
  void _beforeCameraDraw();
  void _afterCameraDraw();
  void _beforeClearStage();

public:
  /**
   * The scene the component belongs to.
   */
  Scene* scene;

}; // end of class PrePassRendererSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_RENDERER_SCENE_COMPONENT_H
