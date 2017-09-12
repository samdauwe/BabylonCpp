#ifndef BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H

namespace BABYLON {

class BABYLON_SHARED_EXPORT GenericController : public WebVRController {

public:
  GenericController(const VRGamepad& vrGamepad);
  ~GenericController();

  void initControllerMesh(
    Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded);

protected:
  void handleButtonChange(int buttonIdx, const ExtendedGamepadButton& state,
                          const GamepadButtonChanges& changes);

private:
  AbstractMesh* _defaultModel;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_GENERIC_CONTROLLER_H