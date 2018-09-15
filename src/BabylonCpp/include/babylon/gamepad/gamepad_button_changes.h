#ifndef BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H
#define BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H

#include <sstream>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT GamepadButtonChanges {
  bool changed;
  bool pressChanged;
  bool touchChanged;
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

#endif // end of BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H
