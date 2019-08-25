#include <babylon/inputs/input_manager.h>

#include <babylon/actions/abstract_action_manager.h>
#include <babylon/actions/action_event.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info.h>
#include <babylon/events/keyboard_info_pre.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

unsigned int InputManager::DragMovementThreshold = 10;
milliseconds_t InputManager::LongPressDelay   = std::chrono::milliseconds(500);
milliseconds_t InputManager::DoubleClickDelay = std::chrono::milliseconds(300);
bool InputManager::ExclusiveDoubleClickMode   = false;

InputManager::InputManager(Scene* scene)
    : meshUnderPointer{this, &InputManager::get_meshUnderPointer}
    , unTranslatedPointer{this, &InputManager::get_unTranslatedPointer}
    , pointerX{this, &InputManager::get_pointerX, &InputManager::set_pointerX}
    , pointerY{this, &InputManager::get_pointerY, &InputManager::set_pointerY}
    , _wheelEventName{""}
    , _onPointerMove{nullptr}
    , _onPointerDown{nullptr}
    , _onPointerUp{nullptr}
    , _initClickEvent{nullptr}
    , _initActionManager{nullptr}
    , _delayedSimpleClick{nullptr}
    , _delayedSimpleClickTimeout{0}
    , _previousDelayedSimpleClickTimeout{0}
    , _meshPickProceed{false}
    , _currentPickResult{std::nullopt}
    , _previousPickResult{std::nullopt}
    , _totalPointersPressed{0}
    , _doubleClickOccured{false}
    , _pointerOverMesh{nullptr}
    , _pickedDownMesh{nullptr}
    , _pickedUpMesh{nullptr}
    , _pointerX{0}
    , _pointerY{0}
    , _unTranslatedPointerX{0}
    , _unTranslatedPointerY{0}
    , _startingPointerPosition{Vector2{0.f, 0.f}}
    , _previousStartingPointerPosition{Vector2{0.f, 0.f}}
    , _startingPointerTime{high_res_time_point_t()}
    , _previousStartingPointerTime{high_res_time_point_t()}
    , _onKeyDown{nullptr}
    , _onKeyUp{nullptr}
    , _onCanvasFocusObserver{nullptr}
    , _onCanvasBlurObserver{nullptr}
    , _scene{scene}
    , _unTranslatedPointer{Vector2{0.f, 0.f}}
{
}

InputManager::~InputManager()
{
}

AbstractMeshPtr& InputManager::get_meshUnderPointer()
{
  return _pointerOverMesh;
}

Vector2& InputManager::get_unTranslatedPointer()
{
  _unTranslatedPointer = Vector2(_unTranslatedPointerX, _unTranslatedPointerY);
  return _unTranslatedPointer;
}

int InputManager::get_pointerX() const
{
  return _pointerX;
}

void InputManager::set_pointerX(int value)
{
  _pointerX = value;
}

int InputManager::get_pointerY() const
{
  return _pointerY;
}

void InputManager::set_pointerY(int value)
{
  _pointerY = value;
}

void InputManager::_updatePointerPosition(const PointerEvent evt)
{
  auto canvasRect = _scene->getEngine()->getRenderingCanvasClientRect();

  if (!canvasRect) {
    return;
  }

  _pointerX = evt.clientX - (*canvasRect).left;
  _pointerY = evt.clientY - (*canvasRect).top;

  _unTranslatedPointerX = _pointerX;
  _unTranslatedPointerY = _pointerY;
}

void InputManager::_processPointerMove(std::optional<PickingInfo>& pickResult,
                                       const PointerEvent& evt)
{
  auto& scene = *_scene;
  auto canvas = scene.getEngine()->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  canvas->tabIndex = 1;

  // Restore pointer
  canvas->style.cursor = scene.defaultCursor;

  auto isMeshPicked
    = (pickResult && pickResult->hit && pickResult->pickedMesh) ? true : false;
  if (isMeshPicked) {
    scene.setPointerOverMesh(pickResult->pickedMesh);

    if (_pointerOverMesh && _pointerOverMesh->actionManager
        && _pointerOverMesh->actionManager->hasPointerTriggers()) {
      if (!_pointerOverMesh->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverMesh->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = scene.hoverCursor;
      }
    }
  }
  else {
    scene.setPointerOverMesh(nullptr);
  }

  for (const auto& step : scene._pointerMoveStage) {
    pickResult = step.action(_unTranslatedPointerX, _unTranslatedPointerY,
                             pickResult, isMeshPicked, canvas);
  }

  if (pickResult) {
    const auto type = (evt.type == EventType::MOUSE_WHEEL
                       || evt.type == EventType::DOM_MOUSE_SCROLL) ?
                        PointerEventTypes::POINTERWHEEL :
                        PointerEventTypes::POINTERMOVE;

    if (scene.onPointerMove) {
      scene.onPointerMove(evt, pickResult, type);
    }

    if (scene.onPointerObservable.hasObservers()) {
      PointerInfo pi(type, *static_cast<MouseWheelEvent const*>(&evt),
                     *pickResult);
      _setRayOnPointerInfo(pi);
      scene.onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
    }
  }
}

void InputManager::_setRayOnPointerInfo(PointerInfo& pointerInfo)
{
  auto& scene = *_scene;
  if (!pointerInfo.pickInfo._pickingUnavailable) {
    if (!pointerInfo.pickInfo.ray.has_value()) {
      auto identityMatrix = Matrix::Identity();
      if (pointerInfo.type == PointerEventTypes::POINTERWHEEL) {
        pointerInfo.pickInfo.ray
          = scene.createPickingRay(pointerInfo.mouseWheelEvent.offsetX,
                                   pointerInfo.mouseWheelEvent.offsetY,
                                   identityMatrix, scene.activeCamera);
      }
      else if (pointerInfo.type == PointerEventTypes::POINTERMOVE) {
        pointerInfo.pickInfo.ray = scene.createPickingRay(
          pointerInfo.pointerEvent.offsetX, pointerInfo.pointerEvent.offsetY,
          identityMatrix, scene.activeCamera);
      }
    }
  }
}

bool InputManager::_checkPrePointerObservable(
  const std::optional<PickingInfo>& pickResult, const PointerEvent& evt,
  PointerEventTypes type)
{
  auto& scene = *_scene;
  PointerInfoPre pi(type, evt, static_cast<float>(_unTranslatedPointerX),
                    static_cast<float>(_unTranslatedPointerY));
  if (pickResult) {
    pi.ray = (*pickResult).ray;
  }
  scene.onPrePointerObservable.notifyObservers(&pi, static_cast<int>(type));
  if (pi.skipOnPointerObservable) {
    return true;
  }
  else {
    return false;
  }
}

void InputManager::simulatePointerMove(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointermove");

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERMOVE)) {
    return;
  }
  _processPointerMove(pickResult, evt);
}

void InputManager::simulatePointerDown(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointerdown");

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERDOWN)) {
    return;
  }

  _processPointerDown(pickResult, evt);
}

void InputManager::_processPointerDown(std::optional<PickingInfo>& pickResult,
                                       const PointerEvent& evt)
{
  auto& scene = *_scene;
  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh) {
    _pickedDownMesh     = (*pickResult).pickedMesh;
    auto iActionManager = _pickedDownMesh->actionManager;
    if (iActionManager) {
      if (iActionManager->hasPickTriggers()) {
        iActionManager->processTrigger(
          Constants::ACTION_OnPickDownTrigger,
          ActionEvent::CreateNew(_pickedDownMesh, evt));
        switch (evt.button) {
          case MouseButtonType::LEFT:
            iActionManager->processTrigger(
              Constants::ACTION_OnLeftPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::MIDDLE:
            iActionManager->processTrigger(
              Constants::ACTION_OnCenterPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::RIGHT:
            iActionManager->processTrigger(
              Constants::ACTION_OnRightPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          default:
            break;
        }
      }

      if (iActionManager->hasSpecificTrigger(
            Constants::ACTION_OnLongPressTrigger)) {
#if 0
        window.setTimeout(
          (function() {
            var pickResult = pick(
              _unTranslatedPointerX, _unTranslatedPointerY, (mesh
                                                             : AbstractMesh)
              : boolean = > mesh.isPickable && mesh.isVisible && mesh.isReady()
                          && mesh.actionManager
                          && mesh.actionManager.hasSpecificTrigger(
                               ActionManager.OnLongPressTrigger)
                          && mesh == _pickedDownMesh,
                false, cameraToUseForPointers);

            if (pickResult && pickResult.hit && pickResult.pickedMesh) {
              if (_totalPointersPressed != 0
                  && ((new Date().getTime() - _startingPointerTime)
                      > Scene.LongPressDelay)
                  && !_isPointerSwiping()) {
                _startingPointerTime = 0;
                actionManager.processTrigger(
                  ActionManager.OnLongPressTrigger,
                  ActionEvent.CreateNew(pickResult.pickedMesh, evt));
              }
            }
          }).bind(this),
          Scene.LongPressDelay);
#endif
      }
    }
  }
  else {
    for (auto& step : scene._pointerDownStage) {
      pickResult = step.action(_unTranslatedPointerX, _unTranslatedPointerY,
                               pickResult, evt);
    }
  }

  if (pickResult) {
    auto type = PointerEventTypes::POINTERDOWN;

    if (scene.onPointerDown) {
      scene.onPointerDown(evt, pickResult, type);
    }

    if (scene.onPointerObservable.hasObservers()) {
      PointerInfo pi(type, evt, *pickResult);
      _setRayOnPointerInfo(pi);
      scene.onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
    }
  }
}

bool InputManager::_isPointerSwiping() const
{
  return std::abs(_startingPointerPosition.x - _pointerX)
           > InputManager::DragMovementThreshold
         || std::abs(_startingPointerPosition.y - _pointerY)
              > InputManager::DragMovementThreshold;
}

void InputManager::simulatePointerUp(std::optional<PickingInfo>& pickResult,
                                     bool doubleTap)
{
  PointerEvent evt("pointerup");

  ClickInfo clickInfo;

  if (doubleTap) {
    clickInfo.doubleClick = true;
  }
  else {
    clickInfo.singleClick = true;
  }

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERUP)) {
    return;
  }

  _processPointerUp(pickResult, evt, clickInfo);
}

void InputManager::_processPointerUp(std::optional<PickingInfo>& pickResult,
                                     const PointerEvent& evt,
                                     const ClickInfo& clickInfo)
{
  auto& scene = *_scene;
  if (pickResult && pickResult && (*pickResult).pickedMesh) {
    const auto& _pickResult = *pickResult;
    _pickedUpMesh           = _pickResult.pickedMesh;
    if (_pickedDownMesh == _pickedUpMesh) {
      if (scene.onPointerPick) {
        scene.onPointerPick(evt, pickResult);
      }
      if (clickInfo.singleClick() && !clickInfo.ignore()
          && scene.onPointerObservable.hasObservers()) {
        const auto type = PointerEventTypes::POINTERPICK;
        PointerInfo pi(type, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        scene.onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
    }
    auto& actionManager = _pickedUpMesh->actionManager;
    if (actionManager && !clickInfo.ignore()) {
      actionManager->processTrigger(Constants::ACTION_OnPickUpTrigger,
                                    ActionEvent::CreateNew(_pickedUpMesh, evt));

      if (!clickInfo.hasSwiped() && clickInfo.singleClick()) {
        actionManager->processTrigger(
          Constants::ACTION_OnPickTrigger,
          ActionEvent::CreateNew(pickResult->pickedMesh, evt));
      }

      auto doubleClickActionManager
        = _pickedUpMesh->_getActionManagerForTrigger(
          Constants::ACTION_OnDoublePickTrigger);
      if (clickInfo.doubleClick() && doubleClickActionManager) {
        doubleClickActionManager->processTrigger(
          Constants::ACTION_OnDoublePickTrigger,
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
    }
  }
  else {
    if (!clickInfo.ignore()) {
      for (auto& step : scene._pointerUpStage) {
        pickResult = step.action(_unTranslatedPointerX, _unTranslatedPointerY,
                                 pickResult, evt);
      }
    }
  }

  if (_pickedDownMesh && _pickedDownMesh != _pickedUpMesh) {
    auto pickedDownActionManager = _pickedDownMesh->_getActionManagerForTrigger(
      Constants::ACTION_OnPickOutTrigger);
    if (pickedDownActionManager) {
      pickedDownActionManager->processTrigger(
        Constants::ACTION_OnPickOutTrigger,
        ActionEvent::CreateNew(_pickedDownMesh, evt));
    }
  }

  auto type = PointerEventTypes::POINTERUP;
  if (scene.onPointerObservable.hasObservers()) {
    if (!clickInfo.ignore() && !clickInfo.hasSwiped()) {
      if (clickInfo.singleClick()
          && scene.onPointerObservable.hasSpecificMask(
            static_cast<int>(PointerEventTypes::POINTERTAP))) {
        type = PointerEventTypes::POINTERTAP;
      }
      else if (clickInfo.doubleClick
               && scene.onPointerObservable.hasSpecificMask(
                 static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
        type = PointerEventTypes::POINTERDOUBLETAP;
      }
      if (static_cast<int>(type) != 0) {
        PointerInfo pi(type, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        scene.onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
    }

    if (!clickInfo.ignore()) {
      type = PointerEventTypes::POINTERUP;

      PointerInfo pi(type, evt, *pickResult);
      _setRayOnPointerInfo(pi);
      scene.onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
    }
  }

  if (scene.onPointerUp && !clickInfo.ignore()) {
    scene.onPointerUp(evt, pickResult, type);
  }
}

bool InputManager::isPointerCaptured(int pointerId)
{
  return stl_util::contains(_pointerCaptures, pointerId)
         && _pointerCaptures[pointerId];
}

void InputManager::attachControl(bool attachUp, bool attachDown,
                                 bool attachMove)
{
  _initActionManager
    = [this](AbstractActionManagerPtr act,
             const ClickInfo & /*clickInfo*/) -> AbstractActionManagerPtr {
    if (!_meshPickProceed) {
      auto& scene     = *_scene;
      auto pickResult = scene.pick(_unTranslatedPointerX, _unTranslatedPointerY,
                                   scene.pointerDownPredicate, false,
                                   scene.cameraToUseForPointers);
      if (pickResult) {
        _currentPickResult = *pickResult;
      }
      else {
        _currentPickResult = std::nullopt;
      }
      if (pickResult.has_value()) {
        const auto _pickResult = *pickResult;
        act                    = (_pickResult.hit && _pickResult.pickedMesh) ?
                _pickResult.pickedMesh->actionManager :
                nullptr;
      }
      _meshPickProceed = true;
    }
    return act;
  };

  _delayedSimpleClick
    = [this](
        MouseButtonType btn, ClickInfo& clickInfo,
        const std::function<void(const ClickInfo& clickInfo,
                                 const std::optional<PickingInfo>& pickResult)>&
          cb) -> void {
    // double click delay is over and that no double click has been raised
    // since, or the 2 consecutive keys pressed are different
    if (((Time::fpTimeSince<float, std::milli>(_previousStartingPointerTime)
          > InputManager::DoubleClickDelay.count())
         && !_doubleClickOccured)
        || btn != _previousButtonPressed) {
      _doubleClickOccured   = false;
      clickInfo.singleClick = true;
      clickInfo.ignore      = false;
      cb(clickInfo, _currentPickResult);
    }
  };

  _initClickEvent = [this](Observable<PointerInfoPre>& obs1,
                           Observable<PointerInfo>& obs2,
                           const PointerEvent& evt,
                           const std::function<void(
                             const ClickInfo& clickInfo,
                             std::optional<PickingInfo>& pickResult)>& cb) {
    ClickInfo clickInfo;
    _currentPickResult           = std::nullopt;
    AbstractActionManagerPtr act = nullptr;

    auto checkPicking
      = obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs2.hasSpecificMask(
          static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs1.hasSpecificMask(
          static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
        || obs2.hasSpecificMask(
          static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
    if (!checkPicking) {
      act = _initActionManager(act, clickInfo);
      if (act) {
        checkPicking = act->hasPickTriggers();
      }
    }

    auto needToIgnoreNext = false;

    if (checkPicking) {
      auto btn            = evt.button;
      clickInfo.hasSwiped = _isPointerSwiping();

      if (!clickInfo.hasSwiped()) {
        auto checkSingleClickImmediately
          = !InputManager::ExclusiveDoubleClickMode;

        if (!checkSingleClickImmediately) {
          checkSingleClickImmediately
            = !obs1.hasSpecificMask(
                static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
              && !obs2.hasSpecificMask(
                static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));

          if (checkSingleClickImmediately
              && !AbstractActionManager::HasSpecificTrigger(
                Constants::ACTION_OnDoublePickTrigger)) {
            act = _initActionManager(act, clickInfo);
            if (act) {
              checkSingleClickImmediately = !act->hasSpecificTrigger(
                Constants::ACTION_OnDoublePickTrigger);
            }
          }
        }

        if (checkSingleClickImmediately) {
          // single click detected if double click delay is over or two
          // different successive keys pressed without exclusive double click
          // or no double click required
          if (Time::fpTimeSince<long, std::milli>(_previousStartingPointerTime)
                > Scene::DoubleClickDelay().count()
              || btn != _previousButtonPressed) {
            clickInfo.singleClick = true;
            cb(clickInfo, _currentPickResult);
            needToIgnoreNext = true;
          }
        }
        // at least one double click is required to be check and exclusive
        // double click is enabled
        else {
          // wait that no double click has been raised during the double click
          // delay
          _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;
        }

        auto checkDoubleClick
          = obs1.hasSpecificMask(
              static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
            || obs2.hasSpecificMask(
              static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
        if (!checkDoubleClick
            && AbstractActionManager::HasSpecificTrigger(
              Constants::ACTION_OnDoublePickTrigger)) {
          act = _initActionManager(act, clickInfo);
          if (act) {
            checkDoubleClick
              = act->hasSpecificTrigger(Constants::ACTION_OnDoublePickTrigger);
          }
        }
        if (checkDoubleClick) {
          // two successive keys pressed are equal, double click delay is not
          // over and double click has not just occurred
          if (btn == _previousButtonPressed
              && Time::fpTimeSince<long, std::milli>(
                   _previousStartingPointerTime)
                   < Scene::DoubleClickDelay().count()
              && !_doubleClickOccured) {
            // pointer has not moved for 2 clicks, it's a double click
            if (!clickInfo.hasSwiped() && !_isPointerSwiping()) {
              _previousStartingPointerTime = high_res_time_point_t();
              _doubleClickOccured          = true;
              clickInfo.doubleClick        = true;
              clickInfo.ignore             = false;
              if (Scene::ExclusiveDoubleClickMode()
                  && _previousDelayedSimpleClickTimeout.count() > 0) {
                // clearTimeout(_previousDelayedSimpleClickTimeout);
              }
              _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;
              cb(clickInfo, _currentPickResult);
            }
            // if the two successive clicks are too far, it's just two simple
            // clicks
            else {
              _doubleClickOccured                = false;
              _previousStartingPointerTime       = _startingPointerTime;
              _previousStartingPointerPosition.x = _startingPointerPosition.x;
              _previousStartingPointerPosition.y = _startingPointerPosition.y;
              _previousButtonPressed             = btn;
              if (Scene::ExclusiveDoubleClickMode()) {
                if (_previousDelayedSimpleClickTimeout.count() > 0) {
                  // clearTimeout(_previousDelayedSimpleClickTimeout);
                }
                _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;

                cb(clickInfo, _previousPickResult);
              }
              else {
                cb(clickInfo, _currentPickResult);
              }
            }
            needToIgnoreNext = true;
          }
          // just the first click of the double has been raised
          else {
            _doubleClickOccured                = false;
            _previousStartingPointerTime       = _startingPointerTime;
            _previousStartingPointerPosition.x = _startingPointerPosition.x;
            _previousStartingPointerPosition.y = _startingPointerPosition.y;
            _previousButtonPressed             = btn;
          }
        }
      }
    }

    if (!needToIgnoreNext) {
      cb(clickInfo, _currentPickResult);
    }
  };

  _onPointerMove = [this](PointerEvent&& evt) -> void {
    _updatePointerPosition(evt);

    // PreObservable support
    if (_checkPrePointerObservable(
          std::nullopt, evt,
          (evt.type == EventType::MOUSE_WHEEL
           || evt.type == EventType::DOM_MOUSE_SCROLL) ?
            PointerEventTypes::POINTERWHEEL :
            PointerEventTypes::POINTERMOVE)) {
      return;
    }

    auto& scene = *_scene;
    if (!scene.cameraToUseForPointers && !scene.activeCamera()) {
      return;
    }

    if (!scene.pointerMovePredicate) {
      scene.pointerMovePredicate = [this](const AbstractMeshPtr& mesh) -> bool {
        auto& scene = *_scene;
        return mesh->isPickable && mesh->isVisible && mesh->isReady()
               && mesh->isEnabled()
               && (mesh->enablePointerMoveEvents
                   || scene.constantlyUpdateMeshUnderPointer
                   || mesh->actionManager != nullptr)
               && (!scene.cameraToUseForPointers
                   || (scene.cameraToUseForPointers->layerMask
                       & mesh->layerMask())
                        != 0);
      };
    }

    // Meshes
    auto pickResult = scene.pick(_unTranslatedPointerX, _unTranslatedPointerY,
                                 scene.pointerMovePredicate, false,
                                 scene.cameraToUseForPointers);

    _processPointerMove(pickResult, evt);
  };

  _onPointerDown = [this](PointerEvent&& evt) {
    auto& scene = *_scene;
    auto engine = scene.getEngine();
    auto canvas = engine->getRenderingCanvas();

    ++_totalPointersPressed;
    _pickedDownMesh  = nullptr;
    _meshPickProceed = false;

    _updatePointerPosition(evt);

    if (scene.preventDefaultOnPointerDown && canvas) {
      evt.preventDefault();
      canvas->focus();
    }

    _startingPointerPosition.x = static_cast<float>(_pointerX);
    _startingPointerPosition.y = static_cast<float>(_pointerY);
    _startingPointerTime       = Time::highresTimepointNow();

    // PreObservable support
    if (_checkPrePointerObservable(std::nullopt, evt,
                                   PointerEventTypes::POINTERDOWN)) {
      return;
    }

    if (!scene.cameraToUseForPointers && !scene.activeCamera()) {
      return;
    }

    _pointerCaptures[evt.pointerId] = true;

    if (!scene.pointerDownPredicate) {
      scene.pointerDownPredicate = [](const AbstractMeshPtr& mesh) -> bool {
        return mesh->isPickable && mesh->isVisible && mesh->isReady()
               && mesh->isEnabled();
      };
    }

    // Meshes
    _pickedDownMesh = nullptr;
    auto pickResult = scene.pick(_unTranslatedPointerX, _unTranslatedPointerY,
                                 scene.pointerDownPredicate, false,
                                 scene.cameraToUseForPointers);

    _processPointerDown(pickResult, evt);
  };

  _onPointerUp = [this](PointerEvent&& evt) {
    if (_totalPointersPressed == 0) { // We are attaching the pointer up to
                                      // windows because of a bug in FF
      return; // So we need to test it the pointer down was pressed before.
    }

    auto& scene = *_scene;
    auto engine = scene.getEngine();
    auto canvas = engine->getRenderingCanvas();

    --_totalPointersPressed;
    _pickedUpMesh    = nullptr;
    _meshPickProceed = false;

    _updatePointerPosition(evt);

    if (scene.preventDefaultOnPointerUp && canvas) {
      evt.preventDefault();
      canvas->focus();
    }

    _initClickEvent(
      scene.onPrePointerObservable, scene.onPointerObservable, evt,
      [this, evt](const ClickInfo& clickInfo,
                  std::optional<PickingInfo> pickResult) {
        auto& scene = *_scene;
        // PreObservable support
        if (scene.onPrePointerObservable.hasObservers()) {
          if (!clickInfo.ignore) {
            if (!clickInfo.hasSwiped()) {
              if (clickInfo.singleClick()
                  && scene.onPrePointerObservable.hasSpecificMask(
                    static_cast<int>(PointerEventTypes::POINTERTAP))) {
                if (_checkPrePointerObservable(std::nullopt, evt,
                                               PointerEventTypes::POINTERTAP)) {
                  return;
                }
              }
              if (clickInfo.doubleClick
                  && scene.onPrePointerObservable.hasSpecificMask(
                    static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
                if (_checkPrePointerObservable(
                      std::nullopt, evt, PointerEventTypes::POINTERDOUBLETAP)) {
                  return;
                }
              }
            }
          }
          else {
            if (_checkPrePointerObservable(std::nullopt, evt,
                                           PointerEventTypes::POINTERUP)) {
              return;
            }
          }
        }

        if (!stl_util::contains(_pointerCaptures, evt.pointerId)
            || !_pointerCaptures[evt.pointerId]) {
          return;
        }

        _pointerCaptures[evt.pointerId] = false;
        if (!scene.cameraToUseForPointers && !scene.activeCamera()) {
          return;
        }

        if (!scene.pointerUpPredicate) {
          scene.pointerUpPredicate
            = [&scene](const AbstractMeshPtr& mesh) -> bool {
            return mesh->isPickable && mesh->isVisible && mesh->isReady()
                   && mesh->isEnabled()
                   && (!scene.cameraToUseForPointers
                       || (scene.cameraToUseForPointers->layerMask
                           & mesh->layerMask)
                            != 0);
          };
        }

        // Meshes
        if (!_meshPickProceed
            && (AbstractActionManager::HasTriggers()
                || scene.onPointerObservable.hasObservers())) {
          _initActionManager(nullptr, clickInfo);
        }
        if (!pickResult) {
          pickResult = *_currentPickResult;
        }

        _processPointerUp(pickResult, evt, clickInfo);

        _previousPickResult = _currentPickResult;
      });
  };

  _onKeyDown = [this](KeyboardEvent&& evt) {
    auto& scene = *_scene;
    auto type   = KeyboardEventTypes::KEYDOWN;
    if (scene.onPreKeyboardObservable.hasObservers()) {
      KeyboardInfoPre pi(type, evt);
      scene.onPreKeyboardObservable.notifyObservers(&pi,
                                                    static_cast<int>(type));
      if (pi.skipOnPointerObservable) {
        return;
      }
    }

    if (scene.onKeyboardObservable.hasObservers()) {
      KeyboardInfo pi(type, evt);
      scene.onKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
    }

    if (scene.actionManager) {
      scene.actionManager->processTrigger(
        Constants::ACTION_OnKeyDownTrigger,
        ActionEvent::CreateNewFromScene(_scene, evt));
    }
  };

  _onKeyUp = [this](KeyboardEvent&& evt) {
    auto& scene = *_scene;
    auto type   = KeyboardEventTypes::KEYUP;
    if (scene.onPreKeyboardObservable.hasObservers()) {
      KeyboardInfoPre pi(type, evt);
      scene.onPreKeyboardObservable.notifyObservers(&pi,
                                                    static_cast<int>(type));
      if (pi.skipOnPointerObservable) {
        return;
      }
    }

    if (scene.onKeyboardObservable.hasObservers()) {
      KeyboardInfo pi(type, evt);
      scene.onKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
    }

    if (scene.actionManager) {
      scene.actionManager->processTrigger(
        Constants::ACTION_OnKeyUpTrigger,
        ActionEvent::CreateNewFromScene(_scene, evt));
    }
  };

  auto engine = _scene->getEngine();
  auto canvas = engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  canvas->addKeyEventListener(EventType::KEY_DOWN, _onKeyDown, false);
  canvas->addKeyEventListener(EventType::KEY_UP, _onKeyUp, false);

  if (attachMove) {
    canvas->addMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove, false);
    // Wheel
    canvas->addMouseEventListener(EventType::MOUSE_WHEEL, _onPointerMove,
                                  false);
  }

  if (attachDown) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_DOWN, _onPointerDown,
                                  false);
  }

  if (attachUp) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_UP, _onPointerUp,
                                  false);
  }

  canvas->tabIndex = 1;
}

void InputManager::detachControl()
{
  auto canvas = _scene->getEngine()->getRenderingCanvas();
  auto engine = _scene->getEngine();

  if (!canvas) {
    return;
  }

  // Pointer
  canvas->removeMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove);
  canvas->removeMouseEventListener(EventType::MOUSE_BUTTON_DOWN,
                                   _onPointerDown);
  canvas->removeMouseEventListener(EventType::MOUSE_BUTTON_UP, _onPointerUp);

  // Blur / Focus
  if (_onCanvasBlurObserver) {
    engine->onCanvasBlurObservable.remove(_onCanvasBlurObserver);
  }

  if (_onCanvasFocusObserver) {
    engine->onCanvasFocusObservable.remove(_onCanvasFocusObserver);
  }

  // Wheel
  canvas->removeMouseEventListener(EventType::MOUSE_WHEEL, _onPointerMove);
  canvas->removeMouseEventListener(EventType::DOM_MOUSE_SCROLL, _onPointerMove);

  // Keyboard
  canvas->removeKeyEventListener(EventType::KEY_DOWN, _onKeyDown);
  canvas->removeKeyEventListener(EventType::KEY_UP, _onKeyUp);
}

void InputManager::setPointerOverMesh(const AbstractMeshPtr& mesh)
{
  if (_pointerOverMesh == mesh) {
    return;
  }

  AbstractActionManagerPtr actionManager = nullptr;
  if (_pointerOverMesh) {
    actionManager = _pointerOverMesh->_getActionManagerForTrigger(
      Constants::ACTION_OnPointerOutTrigger);
    if (actionManager) {
      actionManager->processTrigger(Constants::ACTION_OnPointerOutTrigger,
                                    ActionEvent::CreateNew(_pointerOverMesh));
    }
  }

  _pointerOverMesh = mesh;
  if (_pointerOverMesh) {
    actionManager = _pointerOverMesh->_getActionManagerForTrigger(
      Constants::ACTION_OnPointerOverTrigger);
    if (actionManager) {
      actionManager->processTrigger(Constants::ACTION_OnPointerOverTrigger,
                                    ActionEvent::CreateNew(_pointerOverMesh));
    }
  }
}

AbstractMeshPtr& InputManager::getPointerOverMesh()
{
  return _pointerOverMesh;
}

} // end of namespace BABYLON
