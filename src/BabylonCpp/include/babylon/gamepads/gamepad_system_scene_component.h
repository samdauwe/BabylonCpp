#ifndef BABYLON_GAMEPADS_GAMEPAD_SYSTEM_SCENE_COMPONENT_POSE_H
#define BABYLON_GAMEPADS_GAMEPAD_SYSTEM_SCENE_COMPONENT_POSE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class GamepadSystemSceneComponent;
using GamepadSystemSceneComponentPtr = std::shared_ptr<GamepadSystemSceneComponent>;

/**
 * @brief Defines the gamepad scene component responsible to manage gamepads in
 * a given scene.
 */
class BABYLON_SHARED_EXPORT GamepadSystemSceneComponent : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_GAMEPAD;

public:
  template <typename... Ts>
  static GamepadSystemSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<GamepadSystemSceneComponent>(
      new GamepadSystemSceneComponent(std::forward<Ts>(args)...));
  }
  ~GamepadSystemSceneComponent() override; // = default

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
  GamepadSystemSceneComponent(Scene* scene);

private:
  void _beforeCameraUpdate();

}; // end of class GamepadSystemSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_GAMEPAD_SYSTEM_SCENE_COMPONENT_POSE_H
