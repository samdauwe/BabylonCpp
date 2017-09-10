#include <babylon/gamepad/controllers/mutable_gamepad_button.h>

namespace BABYLON {

MutableGamepadButton::MutableGamepadButton()
    : _value{0}, _touched{false}, _pressed{false}
{
}

MutableGamepadButton::MutableGamepadButton(int value, bool touched,
                                           bool pressed)
    : _value{value}, _touched{touched}, _pressed{pressed}
{
}

MutableGamepadButton::MutableGamepadButton(const MutableGamepadButton& other)
    : _value{other._value}, _touched{other._touched}, _pressed{other._pressed}
{
}

MutableGamepadButton::MutableGamepadButton(MutableGamepadButton&& other)
    : _value{std::move(other._value)}
    , _touched{std::move(other._touched)}
    , _pressed{std::move(other._pressed)}
{
}

MutableGamepadButton& MutableGamepadButton::
operator=(const MutableGamepadButton& other)
{
  if (&other != this) {
    _value   = other._value;
    _touched = other._touched;
    _pressed = other._pressed;
  }

  return *this;
}

MutableGamepadButton& MutableGamepadButton::
operator=(MutableGamepadButton&& other)
{
  if (&other != this) {
    _value   = std::move(other._value);
    _touched = std::move(other._touched);
    _pressed = std::move(other._pressed);
  }

  return *this;
}

MutableGamepadButton::~MutableGamepadButton()
{
}

int MutableGamepadButton::value() const
{
  return _value;
}

bool MutableGamepadButton::touched() const
{
  return _touched;
}

bool MutableGamepadButton::pressed() const
{
  return _pressed;
}

} // end of namespace BABYLON
