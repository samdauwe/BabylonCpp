#ifndef BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H
#define BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT GamepadButtonChanges {
  bool changed;
  bool pressChanged;
  bool touchChanged;
  bool valueChanged;
}; // end of struct GamepadButtonChanges

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GAMEPAD_BUTTON_CHANGES_H
