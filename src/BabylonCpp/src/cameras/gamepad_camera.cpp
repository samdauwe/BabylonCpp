#include <babylon/cameras/gamepad_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

namespace BABYLON {

bool GamepadCamera::NodeConstructorAdded = false;

void GamepadCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "GamepadCamera", [](const std::string& name, Scene* scene,
                        const std::optional<Json::value>& /*options*/) {
      return GamepadCamera::New(name, Vector3::Zero(), scene);
    });
  GamepadCamera::NodeConstructorAdded = true;
}

//-- Begin properties for backward compatibility for inputs

float GamepadCamera::get_gamepadAngularSensibility() const
{
  auto& inputsAttached = inputs->attached();
  if (stl_util::contains(inputsAttached, "gamepad")
      && inputsAttached["gamepad"]) {
    auto gamepad
      = static_cast<FreeCameraGamepadInput*>(inputsAttached["gamepad"].get());
    if (gamepad) {
      return gamepad->gamepadAngularSensibility;
    }
  }

  return 0.f;
}

void GamepadCamera::set_gamepadAngularSensibility(float value)
{
  auto& inputsAttached = inputs->attached();
  if (stl_util::contains(inputsAttached, "gamepad")
      && inputsAttached["gamepad"]) {
    auto gamepad
      = static_cast<FreeCameraGamepadInput*>(inputsAttached["gamepad"].get());
    if (gamepad) {
      gamepad->gamepadAngularSensibility = value;
    }
  }
}

float GamepadCamera::get_gamepadMoveSensibility() const
{
  auto& inputsAttached = inputs->attached();
  if (stl_util::contains(inputsAttached, "gamepad")
      && inputsAttached["gamepad"]) {
    auto gamepad
      = static_cast<FreeCameraGamepadInput*>(inputsAttached["gamepad"].get());
    if (gamepad) {
      return gamepad->gamepadMoveSensibility;
    }
  }

  return 0.f;
}

void GamepadCamera::set_gamepadMoveSensibility(float value)
{
  auto& inputsAttached = inputs->attached();
  if (stl_util::contains(inputsAttached, "gamepad")
      && inputsAttached["gamepad"]) {
    auto gamepad
      = static_cast<FreeCameraGamepadInput*>(inputsAttached["gamepad"].get());
    if (gamepad) {
      gamepad->gamepadMoveSensibility = value;
    }
  }
}

//-- end properties for backward compatibility for inputs

GamepadCamera::GamepadCamera::GamepadCamera(const std::string& name,
                                            const Vector3& position,
                                            Scene* scene)
    : UniversalCamera{name, position, scene}
    , gamepadAngularSensibility{this,
                                &GamepadCamera::get_gamepadAngularSensibility,
                                &GamepadCamera::set_gamepadAngularSensibility}
    , gamepadMoveSensibility{this, &GamepadCamera::get_gamepadMoveSensibility,
                             &GamepadCamera::set_gamepadMoveSensibility}
{
}

GamepadCamera::~GamepadCamera()
{
}

const std::string GamepadCamera::getClassName() const
{
  return "GamepadCamera";
}

} // end of namespace BABYLON
