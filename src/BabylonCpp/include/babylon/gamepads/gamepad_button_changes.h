#ifndef BABYLON_GAMEPADS_GAMEPAD_BUTTON_CHANGES_H
#define BABYLON_GAMEPADS_GAMEPAD_BUTTON_CHANGES_H

#include <sstream>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief An interface which manages callbacks for gamepad button changes.
 */
struct BABYLON_SHARED_EXPORT GamepadButtonChanges {
  /**
   * Called when a gamepad has been changed
   */
  bool changed;

  /**
   * Called when a gamepad press event has been triggered
   */
  bool pressChanged;

  /**
   * Called when a touch event has been triggered
   */
  bool touchChanged;

  /**
   * Called when a value has changed
   */
  bool valueChanged;

  std::string toString() const
  {
    std::ostringstream oss;
    oss << "{\"Changed\":" << changed << ",\"PressChanged\":" << pressChanged
        << ",\"TouchChanged\":" << touchChanged
        << ",\"ValueChanged\":" << valueChanged << "}";
    return oss.str();
  }
}; // end of struct GamepadButtonChanges

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_GAMEPAD_BUTTON_CHANGES_H
