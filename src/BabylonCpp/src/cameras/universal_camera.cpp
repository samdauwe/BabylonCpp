#include <babylon/cameras/universal_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

namespace BABYLON {

bool UniversalCamera::NodeConstructorAdded = false;

void UniversalCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "FreeCamera", [](const std::string& iName, Scene* scene,
                     const std::optional<json>& /*options*/) {
      // Forcing to use the Universal camera
      return UniversalCamera::New(iName, Vector3::Zero(), scene);
    });
  UniversalCamera::NodeConstructorAdded = true;
}

//-- Begin properties for backward compatibility for inputs

float UniversalCamera::get_gamepadAngularSensibility() const
{
  auto& inputsAttached = inputs->attached;
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

void UniversalCamera::set_gamepadAngularSensibility(float value)
{
  auto& inputsAttached = inputs->attached;
  if (stl_util::contains(inputsAttached, "gamepad")
      && inputsAttached["gamepad"]) {
    auto gamepad
      = static_cast<FreeCameraGamepadInput*>(inputsAttached["gamepad"].get());
    if (gamepad) {
      gamepad->gamepadAngularSensibility = value;
    }
  }
}

float UniversalCamera::get_gamepadMoveSensibility() const
{
  auto& inputsAttached = inputs->attached;
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

void UniversalCamera::set_gamepadMoveSensibility(float value)
{
  auto& inputsAttached = inputs->attached;
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

UniversalCamera::UniversalCamera::UniversalCamera(const std::string& iName,
                                                  const Vector3& iPosition,
                                                  Scene* scene)
    : TouchCamera{iName, iPosition, scene}
    , gamepadAngularSensibility{this,
                                &UniversalCamera::get_gamepadAngularSensibility,
                                &UniversalCamera::set_gamepadAngularSensibility}
    , gamepadMoveSensibility{this, &UniversalCamera::get_gamepadMoveSensibility,
                             &UniversalCamera::set_gamepadMoveSensibility}
{
  inputs->addGamepad();
}

UniversalCamera::~UniversalCamera() = default;

std::string UniversalCamera::getClassName() const
{
  return "UniversalCamera";
}

} // end of namespace BABYLON
