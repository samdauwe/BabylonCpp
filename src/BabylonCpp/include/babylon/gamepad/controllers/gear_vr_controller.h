#ifndef BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GearVRController : public WebVRController {

public:
  static constexpr const char* MODEL_BASE_URL
    = "https://controllers.babylonjs.com/generic/";
  static constexpr const char* MODEL_FILENAME = "generic.babylon";

  // id is 'Gear VR Controller'
  static constexpr const char* GAMEPAD_ID_PREFIX = "Gear VR";

public:
  GearVRController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~GearVRController() override;

  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  void _handleButtonChange(unsigned int buttonIdx,
                           const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

private:
  const array_t<string_t, 2> _buttonIndexToObservableNameMap;

}; // end of class GearVRController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
