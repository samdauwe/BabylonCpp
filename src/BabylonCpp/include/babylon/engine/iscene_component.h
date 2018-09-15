#ifndef BABYLON_ENGINE_ISCENE_COMPONENT_H
#define BABYLON_ENGINE_ISCENE_COMPONENT_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;

/**
 * @brief This represents a scene component.
 *
 * This is used to decouple the dependency the scene is having on the different
 * workloads like layers, post processes...
 */
struct BABYLON_SHARED_EXPORT ISceneComponent
    : public ::std::enable_shared_from_this<ISceneComponent> {

  /**
   * @brief The name of the component. Each component must have a unique name.
   */
  std::string name;

  /**
   * @brief The scene the component belongs to.
   */
  Scene* scene;

  /**
   * @brief Register the component to one instance of a scene.
   */
  virtual void _register() = 0;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  virtual void rebuild() = 0;

  /**
   * @brief Disposes the component and the associated ressources.
   */
  virtual void dispose() = 0;

}; // end of struct ISceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ISCENE_COMPONENT_H
