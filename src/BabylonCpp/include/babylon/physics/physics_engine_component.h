#ifndef BABYLON_PHYSICS_PHYSICS_ENGINE_COMPONENT_H
#define BABYLON_PHYSICS_PHYSICS_ENGINE_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class PhysicsEngineSceneComponent;
using PhysicsEngineSceneComponentPtr
  = std::shared_ptr<PhysicsEngineSceneComponent>;

/**
 * @brief Defines the physics engine scene component responsible to manage a
 * physics engine.
 */
class BABYLON_SHARED_EXPORT PhysicsEngineSceneComponent
    : public ISceneComponent {

public:
  /**
   * The component name helpful to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_PHYSICSENGINE;

public:
  template <typename... Ts>
  static PhysicsEngineSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<PhysicsEngineSceneComponent>(
      new PhysicsEngineSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~PhysicsEngineSceneComponent(); // = default

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

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  PhysicsEngineSceneComponent(Scene* scene);

}; // end of class PhysicsEngineSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_ENGINE_COMPONENT_H
