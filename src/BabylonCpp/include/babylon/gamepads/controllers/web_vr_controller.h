#ifndef BABYLON_GAMEPADS_CONTROLLERS_WEB_VR_CONTROLLER_H
#define BABYLON_GAMEPADS_CONTROLLERS_WEB_VR_CONTROLLER_H

#include <babylon/babylon_api.h>
#include <babylon/gamepads/controllers/extended_gamepad_button.h>
#include <babylon/gamepads/controllers/pose_enabled_controller.h>
#include <babylon/gamepads/gamepad_button_changes.h>
#include <babylon/gamepads/stick_values.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class Scene;

/**
 * @brief Defines the WebVRController object that represents controllers tracked
 * in 3D space.
 */
class BABYLON_SHARED_EXPORT WebVRController : public PoseEnabledController {

public:
  /**
   * @brief Creates a new WebVRController from a gamepad.
   * @param vrGamepad the gamepad that the WebVRController should be created
   * from
   */
  WebVRController(const IBrowserGamepadPtr& vrGamepad);
  ~WebVRController() override;

  /**
   * @brief Fired when a controller button's state has changed.
   * @param callback the callback containing the button that was modified
   */
  void setOnButtonStateChange(
    const std::function<void(int controlledIndex, unsigned int buttonIndex,
                             const ExtendedGamepadButton& state)>& callback);

  /**
   * @brief Updates the state of the controller and mesh based on the current
   * position and rotation of the controller.
   */
  void update() override;

  /**
   * @brief Disposes of th webVRCOntroller.
   */
  void dispose() override;

  /**
   * @brief Loads a mesh and attaches it to the controller.
   * @param scene the scene the mesh should be added to
   * @param meshLoaded callback for when the mesh has been loaded
   */
  virtual void
  initControllerMesh(Scene* scene,
                     const std::function<void(AbstractMesh* mesh)>& meshLoaded)
    = 0;

protected:
  /**
   * @brief The default controller model for the controller.
   */
  AbstractMeshPtr& get_defaultModel();

  /**
   * @brief Function to be called when a button is modified.
   */
  virtual void _handleButtonChange(unsigned int buttonIdx,
                                   const ExtendedGamepadButton& state,
                                   const GamepadButtonChanges& changes)
    = 0;

private:
  void _setButtonValue(std::optional<ExtendedGamepadButton> newState,
                       const std::optional<ExtendedGamepadButton>& currentState,
                       unsigned int buttonIndex);
  GamepadButtonChanges&
  _checkChanges(const ExtendedGamepadButton& newState,
                const ExtendedGamepadButton& currentState);

public:
  // Observables
  /**
   * Fired when the trigger state has changed
   */
  Observable<ExtendedGamepadButton> onTriggerStateChangedObservable;
  /**
   * Fired when the trackpad state has changed
   */
  Observable<ExtendedGamepadButton> onTrackpadChangedObservable;
  /**
   * Fired when the main button state has changed
   */
  Observable<ExtendedGamepadButton> onMainButtonStateChangedObservable;
  /**
   * Fired when the secondary button state has changed
   */
  Observable<ExtendedGamepadButton> onSecondaryButtonStateChangedObservable;
  /**
   * Fired when the pad state has changed
   */
  Observable<ExtendedGamepadButton> onPadStateChangedObservable;
  /**
   * Fired when controllers stick values have changed
   */
  Observable<StickValues> onPadValuesChangedObservable;

  /**
   * X and Y axis corrisponding to the controllers joystick
   */
  StickValues pad;

  /**
   * 'left' or 'right', see
   * https://w3c.github.io/gamepad/extensions.html#gamepadhand-enum
   */
  std::string hand;

  /**
   * The default controller model for the controller
   */
  ReadOnlyProperty<WebVRController, AbstractMeshPtr> defaultModel;

protected:
  /**
   * Internal, the default controller model for the controller
   */
  AbstractMeshPtr _defaultModel;

  /**
   * Array of button availible on the controller
   */
  std::vector<ExtendedGamepadButton> _buttons;

private:
  std::function<void(int controlledIndex, unsigned int buttonIndex,
                     const ExtendedGamepadButton& state)>
    _onButtonStateChange;
  GamepadButtonChanges _changes;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_WEB_VR_CONTROLLER_H
