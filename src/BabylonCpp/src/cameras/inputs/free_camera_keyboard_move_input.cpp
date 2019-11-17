#include <babylon/cameras/inputs/free_camera_keyboard_move_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCameraKeyboardMoveInput::FreeCameraKeyboardMoveInput()
    : _canvas{nullptr}
    , _noPreventDefault{false}
    , _onCanvasBlurObserver{nullptr}
    , _onKeyboardObserver{nullptr}
    , _engine{nullptr}
    , _scene{nullptr}
{
  keysUp.emplace_back(38);
  keysDown.emplace_back(40);
  keysLeft.emplace_back(37);
  keysRight.emplace_back(39);
}

FreeCameraKeyboardMoveInput::~FreeCameraKeyboardMoveInput() = default;

void FreeCameraKeyboardMoveInput::attachControl(ICanvas* canvas,
                                                bool noPreventDefault)
{
  if (_onCanvasBlurObserver) {
    return;
  }

  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  _scene  = camera->getScene();
  _engine = _scene->getEngine();

  _onCanvasBlurObserver = _engine->onCanvasBlurObservable.add(
    [this](Engine*, EventState&) { _keys.clear(); });

  _onKeyboardObserver = _scene->onKeyboardObservable.add(
    [this](KeyboardInfo* info, EventState&) {
      const auto& evt = info->event;

      if (!evt.metaKey) {
        if (info->type == KeyboardEventTypes::KEYDOWN) {
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
        }
        else {
          const auto keyCode = evt.keyCode;
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
        }
      }
    });
}

void FreeCameraKeyboardMoveInput::detachControl(ICanvas* /*canvas*/)
{
  if (_scene) {
    if (_onKeyboardObserver) {
      _scene->onKeyboardObservable.remove(_onKeyboardObserver);
    }
    if (_onCanvasBlurObserver) {
      _engine->onCanvasBlurObservable.remove(_onCanvasBlurObserver);
    }
    _onKeyboardObserver   = nullptr;
    _onCanvasBlurObserver = nullptr;
  }
  _keys.clear();
}

void FreeCameraKeyboardMoveInput::checkInputs()
{
  if (_onKeyboardObserver) {
    // Keyboard
    for (const auto& keyCode : _keys) {
      const auto speed = camera->_computeLocalCameraSpeed();

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

      if (camera->getScene()->useRightHandedSystem()) {
        camera->_localDirection->z *= -1.f;
      }

      camera->getViewMatrix().invertToRef(camera->_cameraTransformMatrix);
      Vector3::TransformNormalToRef(*camera->_localDirection,
                                    camera->_cameraTransformMatrix,
                                    camera->_transformedDirection);
      camera->cameraDirection->addInPlace(camera->_transformedDirection);
    }
  }
}

std::string FreeCameraKeyboardMoveInput::getClassName() const
{
  return "FreeCameraKeyboardMoveInput";
}

void FreeCameraKeyboardMoveInput::_onLostFocus()
{
  _keys.clear();
}

std::string FreeCameraKeyboardMoveInput::getSimpleName() const
{
  return "keyboard";
}

} // end of namespace BABYLON
