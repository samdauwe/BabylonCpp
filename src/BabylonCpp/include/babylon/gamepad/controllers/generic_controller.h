#ifndef BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GenericController : public WebVRController {

private:
  static constexpr const char* MODEL_BASE_URL
    = "https://controllers.babylonjs.com/generic/";
  static constexpr const char* MODEL_FILENAME = "generic.babylon";

public:
  GenericController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~GenericController() override;

  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  void _handleButtonChange(unsigned int buttonIdx,
                           const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H
