#include <babylon/gamepad/generic_pad.h>

#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

GenericPad::GenericPad(const string_t& id, int index,
                       const shared_ptr_t<IBrowserGamepad>& browserGamepad)
    : Gamepad(id, index, browserGamepad)
{
  type     = Gamepad::GENERIC;
  _buttons = Uint32Array(browserGamepad->buttons.size());
}

GenericPad::~GenericPad()
{
}

void GenericPad::setOnbuttondown(
  const ::std::function<void(unsigned int buttonPressed)>& callback)
{
  _onbuttondown = callback;
}

void GenericPad::onbuttonup(
  const ::std::function<void(unsigned int buttonPressed)>& callback)
{
  _onbuttonup = callback;
}

unsigned int GenericPad::_setButtonValue(unsigned int newValue,
                                         unsigned int currentValue,
                                         unsigned int buttonIndex)
{
  if (newValue != currentValue) {
    if (_onbuttondown && newValue == 1) {
      _onbuttondown(buttonIndex);
    }
    if (_onbuttonup && newValue == 0) {
      _onbuttonup(buttonIndex);
    }
  }
  return newValue;
}

void GenericPad::update()
{
  Gamepad::update();
  for (unsigned int index = 0; index < _buttons.size(); ++index) {
    _buttons[index] = _setButtonValue(_browserGamepad->buttons[index].value,
                                      _buttons[index], index);
  }
}

} // end of namespace BABYLON
