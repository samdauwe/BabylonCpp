#include <babylon/gamepad/controllers/web_vr_controller.h>

#include <babylon/gamepad/controllers/extended_gamepad_button.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

WebVRController::WebVRController(
  const std::shared_ptr<IBrowserGamepad>& vrGamepad)
    : PoseEnabledController(vrGamepad)
    , pad{StickValues(0.f, 0.f)}
    , hand{vrGamepad->hand}
    , _changes{GamepadButtonChanges{
        // changed
        false,
        // pressChanged
        false,
        // touchChanged
        false,
        // valueChanged
        false,
      }}
{
  _buttons = vector_t<ExtendedGamepadButton>(vrGamepad->buttons.size());
}

WebVRController::~WebVRController()
{
}

void WebVRController::setOnButtonStateChange(
  const ::std::function<void(int controlledIndex, unsigned int buttonIndex,
                             const ExtendedGamepadButton& state)>& callback)
{
  _onButtonStateChange = callback;
}

void WebVRController::update()
{
  PoseEnabledController::update();
  for (unsigned int index = 0; index < _buttons.size(); ++index) {
    _setButtonValue(_browserGamepad->buttons[index], _buttons[index], index);
  };
  if (leftStick().x != pad.x || leftStick().y != pad.y) {
    pad.x = leftStick().x;
    pad.y = leftStick().y;
    onPadValuesChangedObservable.notifyObservers(pad);
  }
}

void WebVRController::_setButtonValue(
  Nullable<ExtendedGamepadButton> newState,
  const Nullable<ExtendedGamepadButton>& currentState, unsigned int buttonIndex)
{
  if (!newState) {
    newState = ExtendedGamepadButton(0, false, false);
  }
  if (!currentState) {
    _buttons[buttonIndex] = ExtendedGamepadButton(
      (*newState).value(), (*newState).touched(), (*newState).pressed());
    return;
  }
  _checkChanges(*newState, *currentState);
  if (_changes.changed) {
    if (_onButtonStateChange) {
      _onButtonStateChange(index, buttonIndex, *newState);
    }

    handleButtonChange(buttonIndex, *newState, _changes);
  }
  _buttons[buttonIndex].setPressed((*newState).pressed());
  _buttons[buttonIndex].setTouched((*newState).touched());
  // oculus triggers are never 0, thou not touched.
  _buttons[buttonIndex].setValue(
    (*newState).value() < 0.00000001 ? 0 : (*newState).value());
}

GamepadButtonChanges&
WebVRController::_checkChanges(const ExtendedGamepadButton& newState,
                               const ExtendedGamepadButton& currentState)
{
  _changes.pressChanged = newState.pressed() != currentState.pressed();
  _changes.touchChanged = newState.touched() != currentState.touched();
  _changes.valueChanged = newState.value() != currentState.value();
  _changes.changed
    = _changes.pressChanged || _changes.touchChanged || _changes.valueChanged;
  return _changes;
}

} // end of namespace BABYLON
