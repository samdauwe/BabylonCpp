#ifndef BABYLON_GAMEPAD_CONTROLLERS_WEB_VR_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_WEB_VR_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/pose_enabled_controller.h>
#include <babylon/gamepad/gamepad_button_changes.h>
#include <babylon/gamepad/stick_values.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT WebVRController : public PoseEnabledController {

public:
  WebVRController(const std::shared_ptr<IBrowserGamepad>& vrGamepad);
  ~WebVRController();

  void setOnButtonStateChange(
    const ::std::function<void(int controlledIndex, unsigned int buttonIndex,
                               const ExtendedGamepadButton& state)>& callback);
  void update() override;

  virtual void initControllerMesh(
    Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
    = 0;

protected:
  virtual void handleButtonChange(unsigned int buttonIdx,
                                  const ExtendedGamepadButton& state,
                                  const GamepadButtonChanges& changes)
    = 0;

private:
  void _setButtonValue(Nullable<ExtendedGamepadButton> newState,
                       const Nullable<ExtendedGamepadButton>& currentState,
                       unsigned int buttonIndex);
  GamepadButtonChanges&
  _checkChanges(const ExtendedGamepadButton& newState,
                const ExtendedGamepadButton& currentState);

public:
  Observable<ExtendedGamepadButton> onTriggerStateChangedObservable;

  Observable<ExtendedGamepadButton> onMainButtonStateChangedObservable;

  Observable<ExtendedGamepadButton> onSecondaryButtonStateChangedObservable;

  Observable<ExtendedGamepadButton> onPadStateChangedObservable;
  Observable<StickValues> onPadValuesChangedObservable;

  StickValues pad;
  // 'left' or 'right', see
  // https://w3c.github.io/gamepad/extensions.html#gamepadhand-enum
  std::string hand;

protected:
  std::vector<ExtendedGamepadButton> _buttons;

private:
  ::std::function<void(int controlledIndex, unsigned int buttonIndex,
                       const ExtendedGamepadButton& state)>
    _onButtonStateChange;
  GamepadButtonChanges _changes;

}; // end of class GenericController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_WEB_VR_CONTROLLER_H
