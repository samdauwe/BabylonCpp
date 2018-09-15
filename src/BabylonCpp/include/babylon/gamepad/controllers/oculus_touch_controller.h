#ifndef BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H

#include <babylon/babylon_api.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

/**
 * @brief Oculus Touch Controller.
 */
class BABYLON_SHARED_EXPORT OculusTouchController : public WebVRController {

public:
  /**
   * Base Url for the controller model.
   */
  static constexpr const char* MODEL_BASE_URL
    = "https://controllers.babylonjs.com/oculus/";
  /**
   * File name for the left controller model.
   */
  static constexpr const char* MODEL_LEFT_FILENAME = "left.babylon";
  /**
   * File name for the right controller model.
   */
  static constexpr const char* MODEL_RIGHT_FILENAME = "right.babylon";

public:
  /**
   * @brief Creates a new OculusTouchController from a gamepad.
   * @param vrGamepad the gamepad that the controller should be created from
   */
  OculusTouchController(const std::shared_ptr<IBrowserGamepad>& vrGamepad);
  ~OculusTouchController() override;

  /**
   * @brief Implements abstract method on WebVRController class, loading
   * controller meshes and calling this.attachToMesh if successful.
   * @param scene scene in which to add meshes
   * @param meshLoaded optional callback function that will be called if the
   * mesh loads successfully.
   */
  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  /**
   * @brief Fired when the A button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onAButtonStateChangedObservable();

  /**
   * @brief Fired when the B button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onBButtonStateChangedObservable();

  /**
   * @brief Fired when the X button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onXButtonStateChangedObservable();

  /**
   * @brief Fired when the Y button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onYButtonStateChangedObservable();

  /**
   * @brief Called once for each button that changed state since the last frame.
   * 0) thumb stick (touch, press, value = pressed (0,1)). value is in
   * this.leftStick 1) index trigger (touch (?), press (only when value > 0.1),
   * value 0 to 1) 2) secondary trigger (same) 3) A (right) X (left), touch,
   * pressed = value 4) B / Y 5) thumb rest
   * @param buttonIdx Which button index changed
   * @param state New state of the button
   * @param changes Which properties on the state changed since last frame
   */
  void _handleButtonChange(unsigned int buttonIdx,
                           const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

public:
  /**
   * Fired when the secondary trigger on this controller is modified
   */
  Observable<ExtendedGamepadButton> onSecondaryTriggerStateChangedObservable;

  /**
   * Fired when the thumb rest on this controller is modified
   */
  Observable<ExtendedGamepadButton> onThumbRestChangedObservable;

  /**
   * Fired when the A button on this controller is modified
   */
  ReadOnlyProperty<OculusTouchController, Observable<ExtendedGamepadButton>>
    onAButtonStateChangedObservable;

  /**
   * Fired when the B button on this controller is modified
   */
  ReadOnlyProperty<OculusTouchController, Observable<ExtendedGamepadButton>>
    onBButtonStateChangedObservable;

  /**
   * Fired when the X button on this controller is modified
   */
  ReadOnlyProperty<OculusTouchController, Observable<ExtendedGamepadButton>>
    onXButtonStateChangedObservable;

  /**
   * Fired when the Y button on this controller is modified
   */
  ReadOnlyProperty<OculusTouchController, Observable<ExtendedGamepadButton>>
    onYButtonStateChangedObservable;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H
