#ifndef BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ViveController : public WebVRController {

public:
  ViveController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~ViveController();

  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

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
  void handleButtonChange(unsigned int buttonIdx,
                          const ExtendedGamepadButton& state,
                          const GamepadButtonChanges& changes) override;

private:
  AbstractMesh* _defaultModel;

}; // end of class ViveController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_VIVE_CONTROLLER_H
