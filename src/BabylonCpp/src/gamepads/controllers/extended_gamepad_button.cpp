#include <babylon/gamepads/controllers/extended_gamepad_button.h>

namespace BABYLON {

ExtendedGamepadButton::ExtendedGamepadButton(int value, bool touched,
                                             bool pressed)
    : MutableGamepadButton(value, touched, pressed)
{
}

ExtendedGamepadButton::~ExtendedGamepadButton() = default;

void ExtendedGamepadButton::setValue(int value)
{
  _value = value;
}

void ExtendedGamepadButton::setTouched(bool value)
{
  _touched = value;
}

void ExtendedGamepadButton::setPressed(bool value)
{
  _pressed = value;
}

} // end of namespace BABYLON
