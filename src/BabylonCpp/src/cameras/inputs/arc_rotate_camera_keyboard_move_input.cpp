#include <babylon/cameras/inputs/arc_rotate_camera_keyboard_move_input.h>

#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

ArcRotateCameraKeyboardMoveInput::ArcRotateCameraKeyboardMoveInput()
    : _canvas{nullptr}, _noPreventDefault{false}
{
  keysUp.emplace_back(38);
  keysDown.emplace_back(40);
  keysLeft.emplace_back(37);
  keysRight.emplace_back(39);
}

ArcRotateCameraKeyboardMoveInput::~ArcRotateCameraKeyboardMoveInput()
{
}

void ArcRotateCameraKeyboardMoveInput::attachControl(ICanvas* canvas,
                                                     bool noPreventDefault)
{
  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  canvas->tabIndex = 1;

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

  _onLostFocus = [&](const FocusEvent& /*e*/) { _keys.clear(); };
}

void ArcRotateCameraKeyboardMoveInput::detachControl(ICanvas* /*canvas*/)
{
  _keys.clear();
  _onKeyDown   = nullptr;
  _onKeyUp     = nullptr;
  _onLostFocus = nullptr;
}

void ArcRotateCameraKeyboardMoveInput::checkInputs()
{
  if (_onKeyDown) {
    for (const auto& keyCode : _keys) {
      if (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
          != keysLeft.end()) {
        camera->inertialAlphaOffset -= 0.01f;
      }
      else if (std::find(keysUp.begin(), keysUp.end(), keyCode)
               != keysUp.end()) {
        camera->inertialAlphaOffset -= 0.01f;
      }
      else if (std::find(keysRight.begin(), keysRight.end(), keyCode)
               != keysRight.end()) {
        camera->inertialAlphaOffset += 0.01f;
      }
      else if (std::find(keysDown.begin(), keysDown.end(), keyCode)
               != keysDown.end()) {
        camera->inertialBetaOffset += 0.01f;
      }
    }
  }
}

const char* ArcRotateCameraKeyboardMoveInput::getTypeName() const
{
  return "ArcRotateCameraKeyboardMoveInput";
}

const char* ArcRotateCameraKeyboardMoveInput::getSimpleName() const
{
  return "keyboard";
}

} // end of namespace BABYLON
