#ifndef BABYLON_GAMEPAD_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H
#define BABYLON_GAMEPAD_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MutableGamepadButton : public IGamepadButton {

public:
  MutableGamepadButton();
  MutableGamepadButton(int value, bool touched, bool pressed);
  MutableGamepadButton(const MutableGamepadButton& other);
  MutableGamepadButton(MutableGamepadButton&& other);
  MutableGamepadButton& operator=(const MutableGamepadButton& other);
  MutableGamepadButton& operator=(MutableGamepadButton&& other);
  ~MutableGamepadButton();

  int value() const;
  bool touched() const;
  bool pressed() const;

protected:
  int _value;
  bool _touched;
  bool _pressed;

}; // end of class MutableGamepadButton

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H
