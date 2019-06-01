#include <babylon/gamepads/generic_pad.h>

#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

GenericPad::GenericPad(const std::string& iId, int iIndex,
                       const IBrowserGamepadPtr& iBrowserGamepad)
    : Gamepad(iId, iIndex, iBrowserGamepad)
{
  type     = Gamepad::GENERIC;
  _buttons = Uint32Array(browserGamepad->buttons.size());
}

GenericPad::~GenericPad()
{
}

void GenericPad::onbuttondown(
  const std::function<void(unsigned int buttonPressed)>& callback)
{
  _onbuttondown = callback;
}

void GenericPad::onbuttonup(
  const std::function<void(unsigned int buttonPressed)>& callback)
{
  _onbuttonup = callback;
}

unsigned int GenericPad::_setButtonValue(unsigned int newValue,
                                         unsigned int currentValue,
                                         unsigned int buttonIndex)
{
  if (newValue != currentValue) {
    if (newValue == 1) {
      if (_onbuttondown) {
        _onbuttondown(buttonIndex);
      }

      onButtonDownObservable.notifyObservers(&buttonIndex);
    }
    if (newValue == 0) {
      if (_onbuttonup) {
        _onbuttonup(buttonIndex);
      }

      onButtonUpObservable.notifyObservers(&buttonIndex);
    }
  }
  return newValue;
}

void GenericPad::update()
{
  Gamepad::update();
  for (unsigned int _index = 0; _index < _buttons.size(); ++_index) {
    _buttons[_index] = _setButtonValue(browserGamepad->buttons[_index].value,
                                       _buttons[_index], _index);
  }
}

void GenericPad::dispose()
{
  Gamepad::dispose();
  onButtonDownObservable.clear();
  onButtonUpObservable.clear();
}

} // end of namespace BABYLON
