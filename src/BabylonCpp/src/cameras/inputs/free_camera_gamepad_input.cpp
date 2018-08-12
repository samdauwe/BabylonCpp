#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

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

FreeCameraGamepadInput::~FreeCameraGamepadInput()
{
}

void FreeCameraGamepadInput::attachControl(ICanvas* /*canvas*/,
                                           bool /*noPreventDefault*/)
{
}

void FreeCameraGamepadInput::detachControl(ICanvas* /*canvas*/)
{
}

void FreeCameraGamepadInput::checkInputs()
{
}

const char* FreeCameraGamepadInput::getClassName() const
{
  return "FreeCameraGamepadInput";
}

const char* FreeCameraGamepadInput::getSimpleName() const
{
  return "gamepad";
}

} // end of namespace BABYLON
