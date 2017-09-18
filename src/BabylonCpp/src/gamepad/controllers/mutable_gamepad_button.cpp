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
    : _value{::std::move(other._value)}
    , _touched{::std::move(other._touched)}
    , _pressed{::std::move(other._pressed)}
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
    _value   = ::std::move(other._value);
    _touched = ::std::move(other._touched);
    _pressed = ::std::move(other._pressed);
  }

  return *this;
}

MutableGamepadButton::~MutableGamepadButton()
{
}

string_t MutableGamepadButton::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
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

std::ostream& operator<<(std::ostream& os,
                         const MutableGamepadButton& mutableGamepadButton)
{
  os << "{\"Value\":" << mutableGamepadButton._value
     << ",\"Touched\":" << mutableGamepadButton._touched
     << ",\"Pressed\":" << mutableGamepadButton._pressed << "}";
  return os;
}

bool operator==(const MutableGamepadButton& lhs,
                const MutableGamepadButton& rhs)
{
  return (lhs._value == rhs._value) && (lhs._touched == rhs._touched)
         && (lhs._pressed == rhs._pressed);
}

bool operator!=(const MutableGamepadButton& lhs,
                const MutableGamepadButton& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
