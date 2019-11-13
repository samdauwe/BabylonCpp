#include <babylon/cameras/free_camera_inputs_manager.h>

#include <babylon/cameras/inputs/free_camera_gamepad_input.h>
#include <babylon/cameras/inputs/free_camera_keyboard_move_input.h>
#include <babylon/cameras/inputs/free_camera_mouse_input.h>
#include <babylon/cameras/inputs/free_camera_touch_input.h>

namespace BABYLON {

FreeCameraInputsManager::FreeCameraInputsManager(FreeCamera* iCamera)
    : CameraInputsManager{iCamera}, _mouseInput{nullptr}
{
}

FreeCameraInputsManager::~FreeCameraInputsManager() = default;

FreeCameraInputsManager& FreeCameraInputsManager::addKeyboard()
{
  add(std::make_shared<FreeCameraKeyboardMoveInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addMouse(bool touchEnabled)
{
  if (!_mouseInput) {
    _mouseInput = std::make_shared<FreeCameraMouseInput>(touchEnabled);
    add(_mouseInput);
  }

  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::removeMouse()
{
  if (_mouseInput) {
    remove(_mouseInput.get());
  }
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addDeviceOrientation()
{
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addGamepad()
{
  add(std::make_shared<FreeCameraGamepadInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addTouch()
{
  add(std::make_shared<FreeCameraTouchInput>());
  return *this;
}

FreeCameraInputsManager& FreeCameraInputsManager::addVirtualJoystick()
{
  return *this;
}

void FreeCameraInputsManager::clear()
{
  CameraInputsManager<FreeCamera>::clear();
  _mouseInput = nullptr;
}

} // end of namespace BABYLON
