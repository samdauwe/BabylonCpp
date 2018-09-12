#include <babylon/cameras/inputs/arc_rotate_camera_pointers_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

ArcRotateCameraPointersInput::ArcRotateCameraPointersInput()
    : buttons{{MouseButtonType::LEFT, MouseButtonType::MIDDLE,
               MouseButtonType::RIGHT}}
    , angularSensibilityX{1000.f}
    , angularSensibilityY{1000.f}
    , pinchPrecision{12.f}
    , pinchDeltaPercentage{0.f}
    , panningSensibility{1000.f}
    , multiTouchPanning{true}
    , multiTouchPanAndZoom{true}
    , pinchInwards{true}
    , _canvas{nullptr}
    , _engine{nullptr}
    , _noPreventDefault{false}
    , _isPanClick{false}
    , _observer{nullptr}
    , _onKeyDown{nullptr}
    , _onKeyUp{nullptr}
    , _onMouseMove{nullptr}
    , _onGesture{nullptr}
    , _onLostFocus{nullptr}
    , _onContextMenu{nullptr}
    , _cacheSoloPointerDefined{false}
    , _pointADefined{false}
    , _pointBDefined{false}
    , _previousPinchSquaredDistance{0.f}
    , _initialDistance{0.f}
    , _twoFingerActivityCount{0}
    , _previousMultiTouchPanPosition{MultiTouchPanPosition{0, 0, false, false}}
{
}

ArcRotateCameraPointersInput::~ArcRotateCameraPointersInput()
{
}

void ArcRotateCameraPointersInput::attachControl(ICanvas* canvas,
                                                 bool noPreventDefault)
{
  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  _engine                        = camera->getEngine();
  _previousPinchSquaredDistance  = 0.f;
  _initialDistance               = 0.f;
  _twoFingerActivityCount        = 0;
  _previousMultiTouchPanPosition = MultiTouchPanPosition{
    0,     // x
    0,     // y
    false, // isPaning
    false  // isPinching
  };

  _pointerInput = [this](PointerInfo* p, EventState&) {
    auto evt = p->pointerEvent;

    if (_engine->isInVRExclusivePointerMode()) {
      return;
    }

    if (p->type != PointerEventTypes::POINTERMOVE
        && (::std::find(buttons.begin(), buttons.end(), evt.button)
            == buttons.end())) {
      return;
    }

    if (p->type == PointerEventTypes::POINTERDOWN) {
      if (evt.srcElement) {
        evt.srcElement->setPointerCapture(evt.pointerId);
      }

      // Manage panning with pan button click
      _isPanClick = evt.button == camera->_panningMouseButton;

      // manage pointers
      _cacheSoloPointer.x         = evt.clientX;
      _cacheSoloPointer.y         = evt.clientY;
      _cacheSoloPointer.pointerId = evt.pointerId;
      _cacheSoloPointer.type      = evt.pointerType;
      _cacheSoloPointerDefined    = true;

      if (!_pointADefined) {
        _pointA        = _cacheSoloPointer;
        _pointADefined = true;
      }
      else if (!_pointBDefined) {
        _pointB        = _cacheSoloPointer;
        _pointBDefined = true;
      }
      if (!_noPreventDefault) {
        evt.preventDefault();
        _canvas->focus();
      }
    }
    else if (p->type == PointerEventTypes::POINTERDOUBLETAP) {
      camera->restoreState();
    }
    else if (p->type == PointerEventTypes::POINTERUP) {
      if (evt.srcElement) {
        evt.srcElement->releasePointerCapture(evt.pointerId);
      }

      _cacheSoloPointerDefined                  = false;
      _previousPinchSquaredDistance             = 0;
      _previousMultiTouchPanPosition.isPaning   = false;
      _previousMultiTouchPanPosition.isPinching = false;
      _twoFingerActivityCount                   = 0;
      _initialDistance                          = 0;

      // would be better to use pointers.remove(evt.pointerId) for multitouch
      // gestures,
      // but emptying completly pointers collection is required to fix a bug on
      // iPhone :
      // when changing orientation while pinching camera, one pointer stay
      // pressed forever if we don't release all pointers
      // will be ok to put back pointers.remove(evt.pointerId); when iPhone bug
      // corrected
      if (_engine->_badOS) {
        _pointADefined = _pointBDefined = false;
      }
      else {
        // only remove the impacted pointer in case of multitouch allowing on
        // most platforms switching from rotate to zoom and pan seamlessly.
        if (_pointBDefined && _pointADefined
            && _pointA.pointerId == evt.pointerId) {
          _pointA           = _pointB;
          _pointADefined    = true;
          _pointBDefined    = false;
          _cacheSoloPointer = ArcRotateCameraPointer{
            _pointA.x, _pointA.y, _pointA.pointerId, evt.pointerType};
          _cacheSoloPointerDefined = true;
        }
        else if (_pointADefined && _pointBDefined
                 && _pointB.pointerId == evt.pointerId) {
          _pointBDefined    = false;
          _cacheSoloPointer = ArcRotateCameraPointer{
            _pointA.x, _pointA.y, _pointA.pointerId, evt.pointerType};
          _cacheSoloPointerDefined = true;
        }
        else {
          _pointADefined = _pointBDefined = false;
        }
      }

      if (!_noPreventDefault) {
        evt.preventDefault();
      }
    }
    else if (p->type == PointerEventTypes::POINTERMOVE) {
      if (!_noPreventDefault) {
        evt.preventDefault();
      }

      // One button down
      if (_pointADefined && !_pointBDefined) {
        if (!stl_util::almost_equal(panningSensibility, 0.f)
            && ((evt.ctrlKey && camera->_useCtrlForPanning) || _isPanClick)) {
          camera->inertialPanningX
            += -static_cast<float>(evt.clientX - _cacheSoloPointer.x)
               / panningSensibility;
          camera->inertialPanningY
            += static_cast<float>(evt.clientY - _cacheSoloPointer.y)
               / panningSensibility;
        }
        else {
          auto offsetX = static_cast<float>(evt.clientX - _cacheSoloPointer.x);
          auto offsetY = static_cast<float>(evt.clientY - _cacheSoloPointer.y);
          camera->inertialAlphaOffset -= offsetX / angularSensibilityX;
          camera->inertialBetaOffset -= offsetY / angularSensibilityY;
        }

        _cacheSoloPointer.x      = evt.clientX;
        _cacheSoloPointer.y      = evt.clientY;
        _cacheSoloPointerDefined = true;
      }

      // Two buttons down: pinch/pan
      else if (_pointADefined && _pointBDefined) {
        // if (noPreventDefault) { evt.preventDefault(); } //if pinch gesture,
        // could be useful to force preventDefault to avoid html page
        // scroll/zoom in some mobile browsers
        auto ed = (_pointA.pointerId == evt.pointerId) ? _pointA : _pointB;
        ed.x    = evt.clientX;
        ed.y    = evt.clientY;
        int direction = pinchInwards ? 1 : -1;
        auto distX    = _pointA.x - _pointB.x;
        auto distY    = _pointA.y - _pointB.y;
        auto pinchSquaredDistance
          = static_cast<float>((distX * distX) + (distY * distY));
        auto pinchDistance = ::std::sqrt(pinchSquaredDistance);

        if (stl_util::almost_equal(_previousPinchSquaredDistance, 0.f)) {
          _initialDistance                 = pinchDistance;
          _previousPinchSquaredDistance    = pinchSquaredDistance;
          _previousMultiTouchPanPosition.x = (_pointA.x + _pointB.x) / 2;
          _previousMultiTouchPanPosition.y = (_pointA.y + _pointB.y) / 2;
          return;
        }

        if (multiTouchPanAndZoom) {
          camera->inertialRadiusOffset
            += (pinchSquaredDistance - _previousPinchSquaredDistance)
               / (pinchPrecision
                  * ((angularSensibilityX + angularSensibilityY) / 2)
                  * direction);

          if (panningSensibility != 0.f) {
            auto pointersCenterX = (_pointA.x + _pointB.x) / 2;
            auto pointersCenterY = (_pointA.y + _pointB.y) / 2;
            auto pointersCenterDistX
              = pointersCenterX - _previousMultiTouchPanPosition.x;
            auto pointersCenterDistY
              = pointersCenterY - _previousMultiTouchPanPosition.y;

            _previousMultiTouchPanPosition.x = pointersCenterX;
            _previousMultiTouchPanPosition.y = pointersCenterY;

            camera->inertialPanningX
              += -(pointersCenterDistX) / (panningSensibility);
            camera->inertialPanningY
              += (pointersCenterDistY) / (panningSensibility);
          }
        }
        else {
          ++_twoFingerActivityCount;

          if (_previousMultiTouchPanPosition.isPinching
              || (_twoFingerActivityCount < 20
                  && std::abs(pinchDistance - _initialDistance)
                       > camera->pinchToPanMaxDistance)) {
            camera->inertialRadiusOffset
              += (pinchSquaredDistance - _previousPinchSquaredDistance)
                 / (pinchPrecision
                    * ((angularSensibilityX + angularSensibilityY) / 2)
                    * direction);
            _previousMultiTouchPanPosition.isPaning   = false;
            _previousMultiTouchPanPosition.isPinching = true;
          }
          else {
            if (_cacheSoloPointer.pointerId == ed.pointerId
                && panningSensibility != 0.f && multiTouchPanning) {
              if (!_previousMultiTouchPanPosition.isPaning) {
                _previousMultiTouchPanPosition.isPaning   = true;
                _previousMultiTouchPanPosition.isPinching = false;
                _previousMultiTouchPanPosition.x          = ed.x;
                _previousMultiTouchPanPosition.y          = ed.y;
                return;
              }

              camera->inertialPanningX
                += -(ed.x - _previousMultiTouchPanPosition.x)
                   / (panningSensibility);
              camera->inertialPanningY
                += (ed.y - _previousMultiTouchPanPosition.y)
                   / (panningSensibility);
            }
          }

          if (_cacheSoloPointer.pointerId == evt.pointerId) {
            _previousMultiTouchPanPosition.x = ed.x;
            _previousMultiTouchPanPosition.y = ed.y;
          }
        }

        _previousPinchSquaredDistance = pinchSquaredDistance;
      }
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN)
                     | static_cast<int>(PointerEventTypes::POINTERUP)
                     | static_cast<int>(PointerEventTypes::POINTERMOVE)
                     | static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));

  _onContextMenu = [](PointerEvent&& evt) { evt.preventDefault(); };

  if (!camera->_useCtrlForPanning) {
    canvas->addMouseEventListener(EventType::CONTEXT_MENU, _onContextMenu,
                                  false);
  }

  _onLostFocus = [this](const FocusEvent&) {
    _pointADefined = _pointBDefined           = false;
    _previousPinchSquaredDistance             = 0.f;
    _previousMultiTouchPanPosition.isPaning   = false;
    _previousMultiTouchPanPosition.isPinching = false;
    _twoFingerActivityCount                   = 0;
    _cacheSoloPointerDefined                  = false;
    _initialDistance                          = 0.f;
  };

  _onMouseMove = [this](MouseEvent& evt) {
    if (!_engine->isPointerLock) {
      return;
    }

    float offsetX = static_cast<float>(evt.movementX);
    float offsetY = static_cast<float>(evt.movementY);

    camera->inertialAlphaOffset -= offsetX / angularSensibilityX;
    camera->inertialBetaOffset -= offsetY / angularSensibilityY;

    if (!_noPreventDefault) {
      evt.preventDefault();
    }
  };

  _onGesture = [this](GestureEvent& e) {
    camera->radius *= e.scale;

    if (!_noPreventDefault) {
      e.stopPropagation();
      e.preventDefault();
    }
  };
}

void ArcRotateCameraPointersInput::detachControl(ICanvas* canvas)
{
  if (canvas && _observer) {
    camera->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;

    _isPanClick  = false;
    pinchInwards = true;

    _onKeyDown     = nullptr;
    _onKeyUp       = nullptr;
    _onMouseMove   = nullptr;
    _onGesture     = nullptr;
    _onLostFocus   = nullptr;
    _onContextMenu = nullptr;

    _cacheSoloPointerDefined = false;
    _pointADefined           = false;
    _pointBDefined           = false;
  }
}

void ArcRotateCameraPointersInput::checkInputs()
{
}

const char* ArcRotateCameraPointersInput::getClassName() const
{
  return "ArcRotateCameraPointersInput";
}

const char* ArcRotateCameraPointersInput::getSimpleName() const
{
  return "pointers";
}

} // end of namespace BABYLON
