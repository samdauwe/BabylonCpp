#include <babylon/cameras/inputs/free_camera_keyboard_move_input.h>

#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCameraKeyboardMoveInput::FreeCameraKeyboardMoveInput()
    : _canvas{nullptr}, _noPreventDefault{false}
{
  keysUp.emplace_back(38);
  keysDown.emplace_back(40);
  keysLeft.emplace_back(37);
  keysRight.emplace_back(39);
}

FreeCameraKeyboardMoveInput::~FreeCameraKeyboardMoveInput()
{
}

void FreeCameraKeyboardMoveInput::attachControl(ICanvas* canvas,
                                                bool noPreventDefault)
{
  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  if (!_onKeyDown) {
    _canvas->tabIndex = 1;

    _onKeyDown = [this](const KeyboardEvent& evt) {
      const int keyCode = evt.keyCode;
      if ((std::find(keysUp.begin(), keysUp.end(), keyCode) != keysUp.end())
          || (std::find(keysDown.begin(), keysDown.end(), keyCode)
              != keysDown.end())
          || (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
              != keysLeft.end())
          || (std::find(keysRight.begin(), keysRight.end(), keyCode)
              != keysRight.end())) {

        if (std::find(_keys.begin(), _keys.end(), keyCode) == _keys.end()) {
          _keys.emplace_back(keyCode);
        }

        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }
    };

    _onKeyUp = [this](const KeyboardEvent& evt) {
      const int keyCode = evt.keyCode;
      if ((std::find(keysUp.begin(), keysUp.end(), keyCode) != keysUp.end())
          || (std::find(keysDown.begin(), keysDown.end(), keyCode)
              != keysDown.end())
          || (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
              != keysLeft.end())
          || (std::find(keysRight.begin(), keysRight.end(), keyCode)
              != keysRight.end())) {

        _keys.erase(std::remove(_keys.begin(), _keys.end(), keyCode),
                    _keys.end());

        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }
    };

    _onLostFocus = [&](const KeyboardEvent& /*e*/) { _keys.clear(); };
  }
}

void FreeCameraKeyboardMoveInput::detachControl(ICanvas* /*canvas*/)
{
  if (_onKeyDown) {
    _keys.clear();
    _onKeyDown   = nullptr;
    _onKeyUp     = nullptr;
    _onLostFocus = nullptr;
  }
}

void FreeCameraKeyboardMoveInput::checkInputs()
{
  if (_onKeyDown) {
    // Keyboard
    for (const auto& keyCode : _keys) {
      const float speed = camera->_computeLocalCameraSpeed();
      if (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
          != keysLeft.end()) {
        camera->_localDirection->copyFromFloats(-speed, 0.f, 0.f);
      }
      else if (std::find(keysUp.begin(), keysUp.end(), keyCode)
               != keysUp.end()) {
        camera->_localDirection->copyFromFloats(0.f, 0.f, speed);
      }
      else if (std::find(keysRight.begin(), keysRight.end(), keyCode)
               != keysRight.end()) {
        camera->_localDirection->copyFromFloats(speed, 0.f, 0.f);
      }
      else if (std::find(keysDown.begin(), keysDown.end(), keyCode)
               != keysDown.end()) {
        camera->_localDirection->copyFromFloats(0.f, 0.f, -speed);
      }

      camera->getViewMatrix().invertToRef(camera->_cameraTransformMatrix);
      Vector3::TransformNormalToRef(*camera->_localDirection,
                                    camera->_cameraTransformMatrix,
                                    camera->_transformedDirection);
      camera->cameraDirection->addInPlace(camera->_transformedDirection);
    }
  }
}

const char* FreeCameraKeyboardMoveInput::getClassName() const
{
  return "FreeCameraKeyboardMoveInput";
}

const char* FreeCameraKeyboardMoveInput::getSimpleName() const
{
  return "keyboard";
}

} // end of namespace BABYLON
