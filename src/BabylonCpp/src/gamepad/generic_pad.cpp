#include <babylon/gamepad/generic_pad.h>

#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

GenericPad::GenericPad(const std::string& id, int index,
                       const std::shared_ptr<IBrowserGamepad>& browserGamepad)
    : Gamepad(id, index, browserGamepad)
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
  for (unsigned int index = 0; index < _buttons.size(); ++index) {
    _buttons[index] = _setButtonValue(browserGamepad->buttons[index].value,
                                      _buttons[index], index);
  }
}

void GenericPad::dispose()
{
  Gamepad::dispose();
  onButtonDownObservable.clear();
  onButtonUpObservable.clear();
}

} // end of namespace BABYLON
