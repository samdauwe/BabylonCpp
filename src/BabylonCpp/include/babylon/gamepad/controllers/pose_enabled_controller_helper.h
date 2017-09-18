#ifndef BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
#define BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PoseEnabledControllerHelper {
  static WebVRController*
  InitiateController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
}; // end of struct PoseEnabledControllerHelper

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
