#include <babylon/cameras/arc_rotate_camera_inputs_manager.h>

#include <babylon/cameras/inputs/arc_rotate_camera_keyboard_move_input.h>
#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>
#include <babylon/cameras/inputs/arc_rotate_camera_pointers_input.h>

namespace BABYLON {

ArcRotateCameraInputsManager::ArcRotateCameraInputsManager(
  ArcRotateCamera* iCamera)
    : CameraInputsManager{iCamera}
{
}

ArcRotateCameraInputsManager::~ArcRotateCameraInputsManager()
{
}

ArcRotateCameraInputsManager& ArcRotateCameraInputsManager::addMouseWheel()
{
  add(std::make_shared<ArcRotateCameraMouseWheelInput>());
  return *this;
}

ArcRotateCameraInputsManager& ArcRotateCameraInputsManager::addPointers()
{
  add(std::make_shared<ArcRotateCameraPointersInput>());
  return *this;
}

ArcRotateCameraInputsManager& ArcRotateCameraInputsManager::addKeyboard()
{
  add(std::make_shared<ArcRotateCameraKeyboardMoveInput>());
  return *this;
}

ArcRotateCameraInputsManager& ArcRotateCameraInputsManager::addGamepad()
{
  // add(std::make_shared<ArcRotateCameraGamepadInput>());
  return *this;
}

} // end of namespace BABYLON
