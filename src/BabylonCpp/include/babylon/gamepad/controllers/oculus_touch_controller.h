#ifndef BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT OculusTouchController : public WebVRController {

private:
  static constexpr const char* MODEL_BASE_URL
    = "https://controllers.babylonjs.com/oculus/";
  static constexpr const char* MODEL_LEFT_FILENAME  = "left.babylon";
  static constexpr const char* MODEL_RIGHT_FILENAME = "right.babylon";

public:
  OculusTouchController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~OculusTouchController();

  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

  // helper getters for left and right hand.
  Observable<ExtendedGamepadButton>& onAButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onBButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onXButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onYButtonStateChangedObservable();

protected:
  /**
   * 0) thumb stick (touch, press, value = pressed (0,1)). value is in
   * this.leftStick
   * 1) index trigger (touch (?), press (only when value > 0.1), value 0 to 1)
   * 2) secondary trigger (same)
   * 3) A (right) X (left), touch, pressed = value
   * 4) B / Y
   * 5) thumb rest
   */
  void handleButtonChange(unsigned int buttonIdx,
                          const ExtendedGamepadButton& state,
                          const GamepadButtonChanges& changes) override;

public:
  Observable<ExtendedGamepadButton> onSecondaryTriggerStateChangedObservable;
  Observable<ExtendedGamepadButton> onThumbRestChangedObservable;

private:
  AbstractMesh* _defaultModel;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_OCULUS_TOUCH_CONTROLLER_H
