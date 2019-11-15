#include <babylon/cameras/inputs/fly_camera_keyboard_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info.h>

namespace BABYLON {

FlyCameraKeyboardInput::FlyCameraKeyboardInput()
    : _canvas{nullptr}
    , _noPreventDefault{false}
    , _onCanvasBlurObserver{nullptr}
    , _onKeyboardObserver{nullptr}
    , _engine{nullptr}
    , _scene{nullptr}
{
  keysForward.emplace_back(87);
  keysBackward.emplace_back(83);
  keysUp.emplace_back(69);
  keysDown.emplace_back(81);
  keysRight.emplace_back(68);
  keysLeft.emplace_back(65);
}

FlyCameraKeyboardInput::~FlyCameraKeyboardInput() = default;

void FlyCameraKeyboardInput::attachControl(ICanvas* canvas,
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

  _onKeyboardObserver = _scene->onKeyboardObservable.add([this](
                                                           KeyboardInfo* info,
                                                           EventState&) {
    const auto& evt = info->event;

    if (info->type == KeyboardEventTypes::KEYDOWN) {
      const auto keyCode = evt.keyCode;
      if ((std::find(keysForward.begin(), keysForward.end(), keyCode)
           != keysForward.end())
          || (std::find(keysBackward.begin(), keysBackward.end(), keyCode)
              != keysBackward.end())
          || (std::find(keysUp.begin(), keysUp.end(), keyCode) != keysUp.end())
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
      if ((std::find(keysForward.begin(), keysForward.end(), keyCode)
           != keysForward.end())
          || (std::find(keysBackward.begin(), keysBackward.end(), keyCode)
              != keysBackward.end())
          || (std::find(keysUp.begin(), keysUp.end(), keyCode) != keysUp.end())
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
  });
}

void FlyCameraKeyboardInput::detachControl(ICanvas* /*canvas*/)
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

const std::string FlyCameraKeyboardInput::getClassName() const
{
  return "FlyCameraKeyboardInput";
}

void FlyCameraKeyboardInput::_onLostFocus(const FocusEvent& /*e*/)
{
  _keys.clear();
}

const std::string FlyCameraKeyboardInput::getSimpleName() const
{
  return "keyboard";
}

void FlyCameraKeyboardInput::checkInputs()
{
  if (_onKeyboardObserver) {
    // Keyboard
    for (const auto& keyCode : _keys) {
      auto speed = camera->_computeLocalCameraSpeed();

      if (std::find(keysForward.begin(), keysForward.end(), keyCode)
          != keysForward.end()) {
        camera->_localDirection->copyFromFloats(0.f, 0.f, speed);
      }
      else if (std::find(keysBackward.begin(), keysBackward.end(), keyCode)
               != keysBackward.end()) {
        camera->_localDirection->copyFromFloats(0.f, 0.f, -speed);
      }
      else if (std::find(keysUp.begin(), keysUp.end(), keyCode)
               != keysUp.end()) {
        camera->_localDirection->copyFromFloats(0.f, speed, 0.f);
      }
      else if (std::find(keysDown.begin(), keysDown.end(), keyCode)
               != keysDown.end()) {
        camera->_localDirection->copyFromFloats(0.f, -speed, 0.f);
      }
      else if (std::find(keysRight.begin(), keysRight.end(), keyCode)
               != keysRight.end()) {
        camera->_localDirection->copyFromFloats(speed, 0.f, 0.f);
      }
      else if (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
               != keysLeft.end()) {
        camera->_localDirection->copyFromFloats(-speed, 0.f, 0.f);
      }

      if (camera->getScene()->useRightHandedSystem()) {
        camera->_localDirection->z *= -1.f;
      }

      camera->getViewMatrix().invertToRef(camera->_cameraTransformMatrix);
      Vector3::TransformNormalToRef(camera->_localDirection.value(),
                                    camera->_cameraTransformMatrix,
                                    camera->_transformedDirection);
      camera->cameraDirection->addInPlace(camera->_transformedDirection);
    }
  }
}

} // end of namespace BABYLON
