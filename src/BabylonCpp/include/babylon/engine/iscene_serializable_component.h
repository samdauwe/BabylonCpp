#ifndef BABYLON_ENGINE_ISCENE_SERIALIZABLE_COMPONENT_H
#define BABYLON_ENGINE_ISCENE_SERIALIZABLE_COMPONENT_H

#include <map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/engine/iscene_component.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class AbstractScene;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief This represents a SERIALIZABLE scene component.
 *
 * This extends Scene Component to add Serialization methods on top.
 */
struct BABYLON_SHARED_EXPORT ISceneSerializableComponent
    : public ISceneComponent {

  /**
   * @brief Adds all the element from the container to the scene.
   * @param container the container holding the elements
   */
  virtual void addFromContainer(AbstractScene* container) = 0;

  /**
   * @brief Removes all the elements in the container from the scene.
   * @param container contains the elements to remove
   */
  virtual void removeFromContainer(AbstractScene* container) = 0;

  /**
   * @brief Serializes the component data to the specified json object.
   * @param serializationObject The object to serialize to
   */
  virtual void serialize(const Json::object& serializationObject) = 0;

}; // end of struct ISceneSerializableComponent

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ISCENE_SERIALIZABLE_COMPONENT_H
