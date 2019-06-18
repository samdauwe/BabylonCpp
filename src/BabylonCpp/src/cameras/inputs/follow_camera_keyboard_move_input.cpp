#include <babylon/cameras/inputs/follow_camera_keyboard_move_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/keyboard_event_types.h>

namespace BABYLON {

FollowCameraKeyboardMoveInput::FollowCameraKeyboardMoveInput()
    : keysHeightOffsetModifierAlt{false}
    , keysHeightOffsetModifierCtrl{false}
    , keysHeightOffsetModifierShift{false}
    , keysRotationOffsetModifierAlt{false}
    , keysRotationOffsetModifierCtrl{false}
    , keysRotationOffsetModifierShift{false}
    , keysRadiusModifierAlt{false}
    , keysRadiusModifierCtrl{false}
    , keysRadiusModifierShift{false}
    , heightSensibility{1.f}
    , rotationSensibility{1.f}
    , radiusSensibility{1.f}
    , _canvas{nullptr}
    , _noPreventDefault{false}
    , _onCanvasBlurObserver{nullptr}
    , _onKeyboardObserver{nullptr}
    , _engine{nullptr}
    , _scene{nullptr}
{
  keysHeightOffsetIncr.emplace_back(38);
  keysHeightOffsetDecr.emplace_back(40);
  keysRotationOffsetIncr.emplace_back(37);
  keysRotationOffsetDecr.emplace_back(39);
  keysRadiusIncr.emplace_back(40);
  keysRadiusDecr.emplace_back(38);
}

FollowCameraKeyboardMoveInput::~FollowCameraKeyboardMoveInput()
{
}

void FollowCameraKeyboardMoveInput::attachControl(ICanvas* canvas,
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

    if (!evt.metaKey) {
      if (info->type == KeyboardEventTypes::KEYDOWN) {
        _ctrlPressed  = evt.ctrlKey;
        _altPressed   = evt.altKey;
        _shiftPressed = evt.shiftKey;

        const auto keyCode = evt.keyCode;
        if ((std::find(keysHeightOffsetIncr.begin(), keysHeightOffsetIncr.end(),
                       keyCode)
             != keysHeightOffsetIncr.end())
            || (std::find(keysHeightOffsetDecr.begin(),
                          keysHeightOffsetDecr.end(), keyCode)
                != keysHeightOffsetDecr.end())
            || (std::find(keysRotationOffsetIncr.begin(),
                          keysRotationOffsetIncr.end(), keyCode)
                != keysRotationOffsetIncr.end())
            || (std::find(keysRotationOffsetDecr.begin(),
                          keysRotationOffsetDecr.end(), keyCode)
                != keysRotationOffsetDecr.end())
            || (std::find(keysRadiusIncr.begin(), keysRadiusIncr.end(), keyCode)
                != keysRadiusIncr.end())
            || (std::find(keysRadiusDecr.begin(), keysRadiusDecr.end(), keyCode)
                != keysRadiusDecr.end())) {

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
        if ((std::find(keysHeightOffsetIncr.begin(), keysHeightOffsetIncr.end(),
                       keyCode)
             != keysHeightOffsetIncr.end())
            || (std::find(keysHeightOffsetDecr.begin(),
                          keysHeightOffsetDecr.end(), keyCode)
                != keysHeightOffsetDecr.end())
            || (std::find(keysRotationOffsetIncr.begin(),
                          keysRotationOffsetIncr.end(), keyCode)
                != keysRotationOffsetIncr.end())
            || (std::find(keysRotationOffsetDecr.begin(),
                          keysRotationOffsetDecr.end(), keyCode)
                != keysRotationOffsetDecr.end())
            || (std::find(keysRadiusIncr.begin(), keysRadiusIncr.end(), keyCode)
                != keysRadiusIncr.end())
            || (std::find(keysRadiusDecr.begin(), keysRadiusDecr.end(), keyCode)
                != keysRadiusDecr.end())) {

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

void FollowCameraKeyboardMoveInput::detachControl(ICanvas* /*canvas*/)
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

void FollowCameraKeyboardMoveInput::checkInputs()
{
  if (_onKeyboardObserver) {
    for (auto keyCode : _keys) {
      if ((std::find(keysHeightOffsetIncr.begin(), keysHeightOffsetIncr.end(),
                     keyCode)
           != keysHeightOffsetIncr.end())
          && _modifierHeightOffset()) {
        camera->heightOffset += heightSensibility;
      }
      else if ((std::find(keysHeightOffsetDecr.begin(),
                          keysHeightOffsetDecr.end(), keyCode)
                != keysHeightOffsetDecr.end())
               && _modifierHeightOffset()) {
        camera->heightOffset -= heightSensibility;
      }
      else if ((std::find(keysRotationOffsetIncr.begin(),
                          keysRotationOffsetIncr.end(), keyCode)
                != keysRotationOffsetIncr.end())
               && _modifierRotationOffset()) {
        camera->rotationOffset += rotationSensibility;
        camera->rotationOffset = std::fmod(camera->rotationOffset, 360.f);
      }
      else if ((std::find(keysRotationOffsetDecr.begin(),
                          keysRotationOffsetDecr.end(), keyCode)
                != keysRotationOffsetDecr.end())
               && _modifierRotationOffset()) {
        camera->rotationOffset -= rotationSensibility;
        camera->rotationOffset = std::fmod(camera->rotationOffset, 360.f);
      }
      else if ((std::find(keysRadiusIncr.begin(), keysRadiusIncr.end(), keyCode)
                != keysRadiusIncr.end())
               && _modifierRadius()) {
        camera->radius += radiusSensibility;
      }
      else if ((std::find(keysRadiusDecr.begin(), keysRadiusDecr.end(), keyCode)
                != keysRadiusDecr.end())
               && _modifierRadius()) {
        camera->radius -= radiusSensibility;
      }
    }
  }
}

const std::string FollowCameraKeyboardMoveInput::getClassName() const
{
  return "FollowCameraKeyboardMoveInput";
}

const std::string FollowCameraKeyboardMoveInput::getSimpleName() const
{
  return "keyboard";
}

bool FollowCameraKeyboardMoveInput::_modifierHeightOffset() const
{
  return (keysHeightOffsetModifierAlt == _altPressed
          && keysHeightOffsetModifierCtrl == _ctrlPressed
          && keysHeightOffsetModifierShift == _shiftPressed);
}

bool FollowCameraKeyboardMoveInput::_modifierRotationOffset() const
{
  return (keysRotationOffsetModifierAlt == _altPressed
          && keysRotationOffsetModifierCtrl == _ctrlPressed
          && keysRotationOffsetModifierShift == _shiftPressed);
}

bool FollowCameraKeyboardMoveInput::_modifierRadius() const
{
  return (keysRadiusModifierAlt == _altPressed
          && keysRadiusModifierCtrl == _ctrlPressed
          && keysRadiusModifierShift == _shiftPressed);
}

} // end of namespace BABYLON
