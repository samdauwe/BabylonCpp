#include <babylon/cameras/fly_camera_inputs_manager.h>

#include <babylon/cameras/inputs/fly_camera_keyboard_input.h>
#include <babylon/cameras/inputs/fly_camera_mouse_input.h>

namespace BABYLON {

FlyCameraInputsManager::FlyCameraInputsManager(FlyCamera* iCamera)
    : CameraInputsManager{iCamera}
{
}

FlyCameraInputsManager::~FlyCameraInputsManager() = default;

FlyCameraInputsManager& FlyCameraInputsManager::addKeyboard()
{
  add(std::make_shared<FlyCameraKeyboardInput>());
  return *this;
}

FlyCameraInputsManager& FlyCameraInputsManager::addMouse(bool touchEnabled)
{
  add(std::make_shared<FlyCameraMouseInput>(touchEnabled));
  return *this;
}

} // end of namespace BABYLON
