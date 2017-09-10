#ifndef BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H
#define BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/mutable_gamepad_button.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ExtendedGamepadButton
  : public MutableGamepadButton {

public:
  ExtendedGamepadButton();
  ExtendedGamepadButton(int value, bool touched, bool pressed);
  ~ExtendedGamepadButton();

  void setValue(int value);
  void setTouched(bool value);
  void setPressed(bool value);

}; // end of class MutableGamepadButton

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H
