#include <babylon/cameras/universal_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

namespace BABYLON {

//-- Begin properties for backward compatibility for inputs

float UniversalCamera::get_gamepadAngularSensibility() const
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

void UniversalCamera::set_gamepadAngularSensibility(float value)
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

float UniversalCamera::get_gamepadMoveSensibility() const
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

void UniversalCamera::set_gamepadMoveSensibility(float value)
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

UniversalCamera::UniversalCamera::UniversalCamera(const std::string& name,
                                                  const Vector3& position,
                                                  Scene* scene)
    : TouchCamera{name, position, scene}
    , gamepadAngularSensibility{this,
                                &UniversalCamera::get_gamepadAngularSensibility,
                                &UniversalCamera::set_gamepadAngularSensibility}
    , gamepadMoveSensibility{this, &UniversalCamera::get_gamepadMoveSensibility,
                             &UniversalCamera::set_gamepadMoveSensibility}
{
  inputs->addGamepad();
}

UniversalCamera::~UniversalCamera()
{
}

const std::string UniversalCamera::getClassName() const
{
  return "UniversalCamera";
}

} // end of namespace BABYLON
