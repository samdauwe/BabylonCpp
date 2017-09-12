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
  if (touchEnabled) {
    return *this;
  }
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addDeviceOrientation()
{
  return *this;
}

} // end of namespace BABYLON
