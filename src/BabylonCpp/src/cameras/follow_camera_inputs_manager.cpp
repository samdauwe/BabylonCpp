#include <babylon/cameras/follow_camera_inputs_manager.h>

#include <babylon/cameras/inputs/follow_camera_keyboard_move_input.h>
#include <babylon/cameras/inputs/follow_camera_mouse_wheel_input.h>
#include <babylon/cameras/inputs/follow_camera_pointers_input.h>
#include <babylon/core/logging.h>

namespace BABYLON {

FollowCameraInputsManager::FollowCameraInputsManager(FollowCamera* iCamera)
    : CameraInputsManager{iCamera}
{
}

FollowCameraInputsManager::~FollowCameraInputsManager() = default;

FollowCameraInputsManager& FollowCameraInputsManager::addKeyboard()
{
  add(std::make_shared<FollowCameraKeyboardMoveInput>());
  return *this;
}

FollowCameraInputsManager& FollowCameraInputsManager::addMouseWheel()
{
  add(std::make_shared<FollowCameraMouseWheelInput>());
  return *this;
}

FollowCameraInputsManager& FollowCameraInputsManager::addPointers()
{
  add(std::make_shared<FollowCameraPointersInput>());
  return *this;
}

FollowCameraInputsManager& FollowCameraInputsManager::addVRDeviceOrientation()
{
  BABYLON_LOG_WARN(
    "FollowCameraInputsManager",
    "DeviceOrientation support not yet implemented for FollowCamera.")
  return *this;
}

} // end of namespace BABYLON
