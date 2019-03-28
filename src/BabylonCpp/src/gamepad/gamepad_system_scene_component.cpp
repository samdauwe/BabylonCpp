#include <babylon/gamepad/gamepad_system_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/gamepad/gamepad_manager.h>

namespace BABYLON {

GamepadSystemSceneComponent::GamepadSystemSceneComponent(Scene* iScene)
{
  ISceneComponent::name = GamepadSystemSceneComponent::name;
  scene                 = iScene;
}

GamepadSystemSceneComponent::~GamepadSystemSceneComponent()
{
}

void GamepadSystemSceneComponent::_register()
{
  scene->_beforeCameraUpdateStage.registerStep(
    SceneComponentConstants::STEP_BEFORECAMERAUPDATE_GAMEPAD, this,
    [this]() { _beforeCameraUpdate(); });
}

void GamepadSystemSceneComponent::rebuild()
{
  // Nothing to do for gamepads
}

void GamepadSystemSceneComponent::dispose()
{
  auto& gamepadManager = scene->gamepadManager();
  if (gamepadManager) {
    gamepadManager->dispose();
    gamepadManager = nullptr;
  }
}

void GamepadSystemSceneComponent::_beforeCameraUpdate()
{
  auto& gamepadManager = scene->gamepadManager();

  if (gamepadManager && gamepadManager->_isMonitoring) {
    gamepadManager->_checkGamepadsStatus();
  }
}

} // end of namespace BABYLON
