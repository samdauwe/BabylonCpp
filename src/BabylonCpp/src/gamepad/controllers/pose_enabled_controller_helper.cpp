#include <babylon/gamepad/controllers/pose_enabled_controller_helper.h>

#include <babylon/core/string.h>
#include <babylon/gamepad/controllers/daydream_controller.h>
#include <babylon/gamepad/controllers/gear_vr_controller.h>
#include <babylon/gamepad/controllers/generic_controller.h>
#include <babylon/gamepad/controllers/oculus_touch_controller.h>
#include <babylon/gamepad/controllers/vive_controller.h>
#include <babylon/gamepad/controllers/windows_motion_controller.h>
#include <babylon/interfaces/ibrowser_gamepad.h>

namespace BABYLON {

WebVRController* PoseEnabledControllerHelper::InitiateController(
  const std::shared_ptr<IBrowserGamepad>& vrGamepad)
{
  // Oculus Touch
  if (String::contains(vrGamepad->id, "Oculus Touch")) {
    return new OculusTouchController(vrGamepad);
  }
  // Windows Mixed Reality controllers
  else if (String::startsWith(vrGamepad->id,
                              WindowsMotionController::GAMEPAD_ID_PREFIX)) {
    return new WindowsMotionController(vrGamepad);
  }
  // HTC Vive
  else if (String::contains(String::toLowerCase(vrGamepad->id), "openvr")) {
    return new ViveController(vrGamepad);
  }
  // Samsung/Oculus Gear VR or Oculus Go
  else if (String::startsWith(vrGamepad->id,
                              GearVRController::GAMEPAD_ID_PREFIX)
           || String::contains(vrGamepad->id, "Oculus Go")) {
    return new GearVRController(vrGamepad);
  }
  // Google Daydream
  else if (String::startsWith(vrGamepad->id,
                              DaydreamController::GAMEPAD_ID_PREFIX)) {
    return new DaydreamController(vrGamepad);
  }
  // Generic
  else {
    return new GenericController(vrGamepad);
  }
}

} // end of namespace BABYLON
