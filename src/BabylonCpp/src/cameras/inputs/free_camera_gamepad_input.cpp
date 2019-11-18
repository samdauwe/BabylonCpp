#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

#include <babylon/engines/scene.h>
#include <babylon/gamepads/gamepad.h>
#include <babylon/gamepads/gamepad_manager.h>

namespace BABYLON {

FreeCameraGamepadInput::FreeCameraGamepadInput()
    : gamepad{nullptr}
    , gamepadAngularSensibility{200.f}
    , gamepadMoveSensibility{40.f}
    , _onGamepadConnectedObserver{nullptr}
    , _onGamepadDisconnectedObserver{nullptr}
    , _cameraTransform{Matrix::Identity()}
    , _deltaTransform{Vector3::Zero()}
    , _vector3{Vector3::Zero()}
    , _vector2{Vector2::Zero()}
{
}

FreeCameraGamepadInput::~FreeCameraGamepadInput() = default;

void FreeCameraGamepadInput::attachControl(ICanvas* /*canvas*/, bool /*noPreventDefault*/)
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

void FreeCameraGamepadInput::detachControl(ICanvas* /*canvas*/)
{
  camera->getScene()->gamepadManager()->onGamepadConnectedObservable.remove(
    _onGamepadConnectedObserver);
  camera->getScene()->gamepadManager()->onGamepadDisconnectedObservable.remove(
    _onGamepadDisconnectedObserver);
  gamepad = nullptr;
}

void FreeCameraGamepadInput::checkInputs()
{
  if (gamepad && gamepad->leftStick()) {
    auto& LSValues    = *gamepad->leftStick();
    auto normalizedLX = LSValues.x / gamepadMoveSensibility;
    auto normalizedLY = LSValues.y / gamepadMoveSensibility;
    LSValues.x        = std::abs(normalizedLX) > 0.005f ? 0.f + normalizedLX : 0.f;
    LSValues.y        = std::abs(normalizedLY) > 0.005f ? 0.f + normalizedLY : 0.f;

    auto& RSValues = gamepad->rightStick();
    if (RSValues) {
      auto normalizedRX = RSValues->x / gamepadAngularSensibility;
      auto normalizedRY = RSValues->y / gamepadAngularSensibility;
      RSValues->x       = std::abs(normalizedRX) > 0.001f ? 0.f + normalizedRX : 0.f;
      RSValues->y       = std::abs(normalizedRY) > 0.001f ? 0.f + normalizedRY : 0.f;
    }
    else {
      RSValues = StickValues{
        0.f, // x
        0.f  // y
      };
    }

    if (!camera->rotationQuaternion) {
      Matrix::RotationYawPitchRollToRef(camera->rotation().y, camera->rotation().x, 0.f,
                                        _cameraTransform);
    }
    else {
      camera->rotationQuaternion->toRotationMatrix(_cameraTransform);
    }

    auto speed = camera->_computeLocalCameraSpeed() * 50.f;
    _vector3.copyFromFloats(LSValues.x * speed, 0.f, -LSValues.y * speed);

    Vector3::TransformCoordinatesToRef(_vector3, _cameraTransform, _deltaTransform);
    camera->cameraDirection->addInPlace(_deltaTransform);
    _vector2.copyFromFloats(RSValues->y, RSValues->x);
    camera->cameraRotation->addInPlace(_vector2);
  }
}

std::string FreeCameraGamepadInput::getClassName() const
{
  return "FreeCameraGamepadInput";
}

std::string FreeCameraGamepadInput::getSimpleName() const
{
  return "gamepad";
}

} // end of namespace BABYLON
