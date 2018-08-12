#include <babylon/cameras/free_camera_inputs_manager.h>

#include <babylon/cameras/inputs/free_camera_keyboard_move_input.h>
#include <babylon/cameras/inputs/free_camera_mouse_input.h>

namespace BABYLON {

FreeCameraInputsManager::FreeCameraInputsManager(FreeCamera* iCamera)
    : CameraInputsManager{iCamera}
{
}

FreeCameraInputsManager::~FreeCameraInputsManager()
{
}

FreeCameraInputsManager& FreeCameraInputsManager::addKeyboard()
{
  add(::std::make_unique<FreeCameraKeyboardMoveInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addMouse(bool touchEnabled)
{
  add(::std::make_unique<FreeCameraMouseInput>(touchEnabled));
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addGamepad()
{
  // add(::std::make_unique<FreeCameraGamepadInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addDeviceOrientation()
{
  // add(::std::make_unique<FreeCameraDeviceOrientationInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addTouch()
{
  // add(::std::make_unique<FreeCameraTouchInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addVirtualJoystick()
{
  // add(::std::make_unique<FreeCameraVirtualJoystickInput>());
  return *this;
}

} // end of namespace BABYLON
