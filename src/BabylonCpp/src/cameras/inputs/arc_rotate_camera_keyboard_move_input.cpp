#include <babylon/cameras/inputs/arc_rotate_camera_keyboard_move_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

ArcRotateCameraKeyboardMoveInput::ArcRotateCameraKeyboardMoveInput()
    : panningSensibility{50.f}
    , zoomingSensibility{25.f}
    , useAltToZoom{true}
    , angularSpeed{0.01f}
    , _canvas{nullptr}
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
  keysReset.emplace_back(220);
}

ArcRotateCameraKeyboardMoveInput::~ArcRotateCameraKeyboardMoveInput() = default;

void ArcRotateCameraKeyboardMoveInput::attachControl(ICanvas* canvas,
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
          _ctrlPressed = evt.ctrlKey;
          _altPressed  = evt.altKey;

          const auto keyCode = evt.keyCode;
          if ((std::find(keysUp.begin(), keysUp.end(), keyCode) != keysUp.end())
              || (std::find(keysDown.begin(), keysDown.end(), keyCode)
                  != keysDown.end())
              || (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
                  != keysLeft.end())
              || (std::find(keysRight.begin(), keysRight.end(), keyCode)
                  != keysRight.end())
              || (std::find(keysReset.begin(), keysReset.end(), keyCode)
                  != keysReset.end())) {

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
                  != keysRight.end())
              || (std::find(keysReset.begin(), keysReset.end(), keyCode)
                  != keysReset.end())) {

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

void ArcRotateCameraKeyboardMoveInput::detachControl(ICanvas* /*canvas*/)
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

void ArcRotateCameraKeyboardMoveInput::checkInputs()
{
  if (_onKeyboardObserver) {
    for (const auto& keyCode : _keys) {
      if (std::find(keysLeft.begin(), keysLeft.end(), keyCode)
          != keysLeft.end()) {
        if (_ctrlPressed && camera->_useCtrlForPanning) {
          camera->inertialPanningX -= 1.f / panningSensibility;
        }
        else {
          camera->inertialAlphaOffset -= angularSpeed;
        }
      }
      else if (std::find(keysUp.begin(), keysUp.end(), keyCode)
               != keysUp.end()) {
        if (_ctrlPressed && camera->_useCtrlForPanning) {
          camera->inertialPanningY += 1.f / panningSensibility;
        }
        else if (_altPressed && useAltToZoom) {
          camera->inertialRadiusOffset += 1.f / zoomingSensibility;
        }
        else {
          camera->inertialBetaOffset -= angularSpeed;
        }
      }
      else if (std::find(keysRight.begin(), keysRight.end(), keyCode)
               != keysRight.end()) {
        if (_ctrlPressed && camera->_useCtrlForPanning) {
          camera->inertialPanningX += 1.f / panningSensibility;
        }
        else {
          camera->inertialAlphaOffset += angularSpeed;
        }
      }
      else if (std::find(keysDown.begin(), keysDown.end(), keyCode)
               != keysDown.end()) {
        if (_ctrlPressed && camera->_useCtrlForPanning) {
          camera->inertialPanningY -= 1.f / panningSensibility;
        }
        else if (_altPressed && useAltToZoom) {
          camera->inertialRadiusOffset -= 1.f / zoomingSensibility;
        }
        else {
          camera->inertialBetaOffset += angularSpeed;
        }
      }
      else if (std::find(keysReset.begin(), keysReset.end(), keyCode)
               != keysReset.end()) {
        if (camera->useInputToRestoreState) {
          camera->restoreState();
        }
      }
    }
  }
}

const std::string ArcRotateCameraKeyboardMoveInput::getClassName() const
{
  return "ArcRotateCameraKeyboardMoveInput";
}

const std::string ArcRotateCameraKeyboardMoveInput::getSimpleName() const
{
  return "keyboard";
}

} // end of namespace BABYLON
