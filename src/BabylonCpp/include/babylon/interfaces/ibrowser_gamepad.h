#ifndef BABYLON_INTERFACES_IBROWSER_GAMEPAD_H
#define BABYLON_INTERFACES_IBROWSER_GAMEPAD_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/gamepad_pose.h>

namespace BABYLON {

struct IGamepadButton {
  unsigned int value;
  bool pressed;
}; // end of struct IGamepadButton

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT IBrowserGamepad {

public:
  string_t id;
  string_t hand;
  int index;
  Float32Array axes;
  vector_t<IGamepadButton> buttons;
  nullable_t<GamepadPose> pose;

}; // end of class IBrowserGamepad

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IBROWSER_GAMEPAD_H
