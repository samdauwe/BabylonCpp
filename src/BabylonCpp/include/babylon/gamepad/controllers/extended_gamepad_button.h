#ifndef BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H
#define BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/mutable_gamepad_button.h>

namespace BABYLON {

/**
 * @brief Defines the ExtendedGamepadButton interface for a gamepad button which
 * includes state provided by a pose controller.
 */
class BABYLON_SHARED_EXPORT ExtendedGamepadButton
    : public MutableGamepadButton {

public:
  ExtendedGamepadButton();
  ExtendedGamepadButton(int value, bool touched, bool pressed);
  ~ExtendedGamepadButton();

  /**
   * @brief Sets if the button/trigger is currently pressed.
   */
  void setValue(int value);

  /**
   * @brief Sets if the button/trigger is currently touched.
   */
  void setTouched(bool value);

  /**
   * @brief Sets the Value of the button/trigger.
   */
  void setPressed(bool value);

}; // end of class MutableGamepadButton

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_EXTENDED_GAMEPAD_BUTTON_H
