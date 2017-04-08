#include <babylon/cameras/inputs/arc_rotate_camera_pointers_input.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/pointer_event_types.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

ArcRotateCameraPointersInput::ArcRotateCameraPointersInput()
    : buttons{{MouseButtonType::LEFT, MouseButtonType::MIDDLE,
               MouseButtonType::RIGHT}}
    , angularSensibilityX{1000.f}
    , angularSensibilityY{1000.f}
    , pinchPrecision{6.f}
    , panningSensibility{50.f}
    , pinchInwards{true}
    , _canvas{nullptr}
    , _engine{nullptr}
    , _noPreventDefault{false}
    , _isPanClick{false}
    , _cacheSoloPointerDefined{false}
    , _pointADefined{false}
    , _pointBDefined{false}
    , _previousPinchDistance{0.f}
{
}

ArcRotateCameraPointersInput::~ArcRotateCameraPointersInput()
{
}

void ArcRotateCameraPointersInput::attachControl(ICanvas* canvas,
                                                 bool noPreventDefault)
{
  _canvas                = canvas;
  _noPreventDefault      = noPreventDefault;
  _previousPinchDistance = 0.f;

  _pointerInput = [this](const PointerInfo& p, const EventState& /*s*/) {
    auto evt = p.pointerEvent;

    if (p.type != PointerEventTypes::POINTERMOVE
        && evt.button == MouseButtonType::UNDEFINED) {
      return;
    }

    if (p.type == PointerEventTypes::POINTERDOWN) {
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
    else if (p.type == PointerEventTypes::POINTERUP) {
      if (evt.srcElement) {
        evt.srcElement->releasePointerCapture(evt.pointerId);
      }

      _cacheSoloPointerDefined = false;
      _previousPinchDistance   = 0.f;

      // would be better to use pointers.remove(evt.pointerId) for multitouch
      // gestures,
      // but emptying completly pointers collection is required to fix a bug on
      // iPhone :
      // when changing orientation while pinching camera, one pointer stay
      // pressed forever if we don't release all pointers
      // will be ok to put back pointers.remove(evt.pointerId); when iPhone bug
      // corrected
      _pointADefined = _pointBDefined = false;

      if (!_noPreventDefault) {
        evt.preventDefault();
      }
    }
    else if (p.type == PointerEventTypes::POINTERMOVE) {
      if (!_noPreventDefault) {
        evt.preventDefault();
      }

      // One button down
      if (_pointADefined && !_pointBDefined) {
        if (!std_util::almost_equal(panningSensibility, 0.f)
            && ((evt.ctrlKey && camera->_useCtrlForPanning)
                || (!camera->_useCtrlForPanning && _isPanClick))) {
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

        _cacheSoloPointer.x = evt.clientX;
        _cacheSoloPointer.y = evt.clientY;
      }

      // Two buttons down: pinch
      else if (_pointADefined && _pointBDefined) {
        // if (noPreventDefault) { evt.preventDefault(); } //if pinch gesture,
        // could be useful to force preventDefault to avoid html page
        // scroll/zoom in some mobile browsers
        // auto ed = (_pointA.pointerId == evt.pointerId) ? _pointA : _pointB;
        // ed.x    = evt.clientX;
        // ed.y    = evt.clientY;
        int direction = pinchInwards ? 1 : -1;
        auto distX    = _pointA.x - _pointB.x;
        auto distY    = _pointA.y - _pointB.y;
        auto pinchSquaredDistance
          = static_cast<float>((distX * distX) + (distY * distY));
        if (std_util::almost_equal(_previousPinchDistance, 0.f)) {
          _previousPinchDistance = pinchSquaredDistance;
          return;
        }

        if (!std_util::almost_equal(pinchSquaredDistance,
                                    _previousPinchDistance)) {
          camera->inertialRadiusOffset
            += (pinchSquaredDistance - _previousPinchDistance)
               / (pinchPrecision
                  * ((angularSensibilityX + angularSensibilityY) / 2.f)
                  * static_cast<float>(direction));
          _previousPinchDistance = pinchSquaredDistance;
        }
      }
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, PointerEventTypes::POINTERDOWN | PointerEventTypes::POINTERUP
                     | PointerEventTypes::POINTERMOVE);

  _onContextMenu = [](PointerEvent& evt) { evt.preventDefault(); };

  if (!camera->_useCtrlForPanning) {
    canvas->addMouseEventListener(EventType::CONTEXT_MENU, _onContextMenu,
                                  false);
  }

  _onLostFocus = [this](const FocusEvent&) {
    _pointADefined = _pointBDefined = false;
    _previousPinchDistance          = 0.f;
    _cacheSoloPointerDefined        = false;
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
