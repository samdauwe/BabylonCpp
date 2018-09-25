#ifndef BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_SCENE_COMPONENT_H
#define BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_SCENE_COMPONENT_H

#include <map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/scene_component_constants.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class AbstractScene;
class Camera;
class LensFlareSystemSceneComponent;
using LensFlareSystemSceneComponentPtr
  = std::shared_ptr<LensFlareSystemSceneComponent>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief Defines the lens flare scene component responsible to manage any lens
 * flares in a given scene.
 */
class BABYLON_SHARED_EXPORT LensFlareSystemSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_LENSFLARESYSTEM;

public:
  template <typename... Ts>
  static LensFlareSystemSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<LensFlareSystemSceneComponent>(
      new LensFlareSystemSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~LensFlareSystemSceneComponent();

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
   * @brief Adds all the element from the container to the scene.
   * @param container the container holding the elements
   */
  void addFromContainer(AbstractScene* container);

  /**
   * @brief Removes all the elements in the container from the scene
   * @param container contains the elements to remove
   */
  void removeFromContainer(AbstractScene* container);

  /**
   * @brief Serializes the component data to the specified json object
   * @param serializationObject The object to serialize to
   */
  void serialize(Json::value& serializationObject);

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * Creates a new instance of the component for the given scene
   * @param scene Defines the scene to register the component in
   */
  LensFlareSystemSceneComponent(Scene* scene);

private:
  void _draw(Camera* camera);

}; // end of class LensFlareSystemSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_SCENE_COMPONENT_H
