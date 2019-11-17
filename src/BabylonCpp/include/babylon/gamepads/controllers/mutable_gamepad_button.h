#ifndef BABYLON_GAMEPADS_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H
#define BABYLON_GAMEPADS_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

/**
 * @brief Defines the MutableGamepadButton interface for the state of a gamepad
 * button.
 */
class BABYLON_SHARED_EXPORT MutableGamepadButton : public IGamepadButton {

public:
  MutableGamepadButton();
  MutableGamepadButton(int value, bool touched, bool pressed);
  MutableGamepadButton(const MutableGamepadButton& other);
  MutableGamepadButton(MutableGamepadButton&& other);
  MutableGamepadButton& operator=(const MutableGamepadButton& other);
  MutableGamepadButton& operator=(MutableGamepadButton&& other);
  ~MutableGamepadButton(); // = default
  [[nodiscard]] std::string toString() const;
  friend std::ostream& operator<<(std::ostream& os,
                                  const MutableGamepadButton& mutableGamepadButton);
  friend bool operator==(const MutableGamepadButton& lhs, const MutableGamepadButton& rhs);
  friend bool operator!=(const MutableGamepadButton& lhs, const MutableGamepadButton& rhs);

  [[nodiscard]] int value() const;
  [[nodiscard]] bool touched() const;
  [[nodiscard]] bool pressed() const;

protected:
  /**
   * Value of the button/trigger
   */
  int _value;
  /**
   * If the button/trigger is currently touched
   */
  bool _touched;
  /**
   * If the button/trigger is currently pressed
   */
  bool _pressed;

}; // end of class MutableGamepadButton

std::ostream& operator<<(std::ostream& os, const MutableGamepadButton& mutableGamepadButton);
bool operator==(const MutableGamepadButton& lhs, const MutableGamepadButton& rhs);
bool operator!=(const MutableGamepadButton& lhs, const MutableGamepadButton& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_MUTABLE_GAMEPAD_BUTTON_H
