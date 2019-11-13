#include <babylon/cameras/inputs/arc_rotate_camera_gamepad_input.h>

#include <babylon/engines/scene.h>
#include <babylon/gamepads/gamepad.h>
#include <babylon/gamepads/gamepad_manager.h>

namespace BABYLON {

ArcRotateCameraGamepadInput::ArcRotateCameraGamepadInput()
    : gamepad{nullptr}
    , gamepadRotationSensibility{80.f}
    , gamepadMoveSensibility{40.f}
    , _onGamepadConnectedObserver{nullptr}
    , _onGamepadDisconnectedObserver{nullptr}
{
}

ArcRotateCameraGamepadInput::~ArcRotateCameraGamepadInput() = default;

void ArcRotateCameraGamepadInput::attachControl(ICanvas* /*canvas*/,
                                                bool /*noPreventDefault*/)
{
  auto& manager               = camera->getScene()->gamepadManager();
  _onGamepadConnectedObserver = manager->onGamepadConnectedObservable.add(
    [this](Gamepad* iGamepad, EventState & /*es*/) -> void {
      if (iGamepad->type != Gamepad::POSE_ENABLED) {
        // prioritize XBOX gamepads.
        if (!gamepad || iGamepad->type == Gamepad::XBOX) {
          gamepad = iGamepad;
        }
      }
    });

  _onGamepadDisconnectedObserver = manager->onGamepadDisconnectedObservable.add(
    [this](Gamepad* iGamepad, EventState & /*es*/) -> void {
      if (gamepad == iGamepad) {
        gamepad = nullptr;
      }
    });

  gamepad = manager->getGamepadByType(Gamepad::XBOX);
}

void ArcRotateCameraGamepadInput::detachControl(ICanvas* /*canvas*/)
{
  camera->getScene()->gamepadManager()->onGamepadConnectedObservable.remove(
    _onGamepadConnectedObserver);
  camera->getScene()->gamepadManager()->onGamepadDisconnectedObservable.remove(
    _onGamepadDisconnectedObserver);
  gamepad = nullptr;
}

void ArcRotateCameraGamepadInput::checkInputs()
{
  if (gamepad) {
    auto& RSValues = gamepad->rightStick();

    if (RSValues.has_value()) {
      if (RSValues->x != 0.f) {
        auto normalizedRX = RSValues->x / gamepadRotationSensibility;
        if (normalizedRX != 0.f && std::abs(normalizedRX) > 0.005f) {
          camera->inertialAlphaOffset += normalizedRX;
        }
      }

      if (RSValues->y != 0.f) {
        auto normalizedRY = RSValues->y / gamepadRotationSensibility;
        if (normalizedRY != 0.f && std::abs(normalizedRY) > 0.005f) {
          camera->inertialBetaOffset += normalizedRY;
        }
      }
    }

    auto& LSValues = gamepad->leftStick();
    if (LSValues.has_value() && LSValues->y != 0.f) {
      auto normalizedLY = LSValues->y / gamepadMoveSensibility;
      if (normalizedLY != 0.f && std::abs(normalizedLY) > 0.005f) {
        camera->inertialRadiusOffset -= normalizedLY;
      }
    }
  }
}

const std::string ArcRotateCameraGamepadInput::getClassName() const
{
  return "ArcRotateCameraGamepadInput";
}

const std::string ArcRotateCameraGamepadInput::getSimpleName() const
{
  return "gamepad";
}

} // end of namespace BABYLON
