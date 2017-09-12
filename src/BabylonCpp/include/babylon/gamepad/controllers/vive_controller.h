#ifndef BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H

namespace BABYLON {

class BABYLON_SHARED_EXPORT ViveController : public WebVRController {

public:
  ViveController(const VRGamepad& vrGamepad);
  ~ViveController();

  void initControllerMesh(
    Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded);

  Observable<ExtendedGamepadButton>& onLeftButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onRightButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onMenuButtonStateChangedObservable();

protected:
  /**
   * Vive mapping:
   * 0: touchpad
   * 1: trigger
   * 2: left AND right buttons
   * 3: menu button
   */
  void handleButtonChange(int buttonIdx, const ExtendedGamepadButton& state,
                          const GamepadButtonChanges& changes);

private:
  AbstractMesh* _defaultModel;

}; // end of class ViveController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H