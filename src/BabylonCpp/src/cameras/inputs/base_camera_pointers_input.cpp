#include <babylon/cameras/inputs/base_camera_pointers_input.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

template <class TCamera>
BaseCameraPointersInput<TCamera>::BaseCameraPointersInput()
    : buttons{{0, 1, 2}}
    , _altKey{false}
    , _ctrlKey{false}
    , _metaKey{false}
    , _shiftKey{false}
    , _pointerInput{nullptr}
    , _observer{nullptr}
    , _onLostFocus{nullptr}
    , pointA{std::nullopt}
    , pointB{std::nullopt}
    , _element{nullptr}
    , _noPreventDefault{false}
    , _engine{nullptr}
    , _previousPinchSquaredDistance{0.f}
    , _previousMultiTouchPanPosition{std::nullopt}
{
}

template <class TCamera>
BaseCameraPointersInput<TCamera>::~BaseCameraPointersInput() = default;

template <class TCamera>
std::string BaseCameraPointersInput<TCamera>::getClassName() const
{
  return "BaseCameraPointersInput";
}

template <class TCamera>
std::string BaseCameraPointersInput<TCamera>::getSimpleName() const
{
  return "pointers";
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::attachControl(bool noPreventDefault)
{
  _noPreventDefault              = noPreventDefault;
  _engine                        = ICameraInput<TCamera>::camera->getEngine();
  _element                       = _engine->getInputElement();
  _previousPinchSquaredDistance  = 0.f;
  _previousMultiTouchPanPosition = std::nullopt;

  pointA = std::nullopt;
  pointB = std::nullopt;

  _altKey         = false;
  _ctrlKey        = false;
  _metaKey        = false;
  _shiftKey       = false;
  _buttonsPressed = 0;

  _pointerInput = [this](PointerInfo* p, EventState& /*es*/) -> void {
    auto& evt    = p->pointerEvent;
    auto isTouch = evt.pointerType == "touch";

    if (p->type != PointerEventTypes::POINTERMOVE
        && (std::find(buttons.begin(), buttons.end(), evt.button) == buttons.end())) {
      return;
    }

    auto& srcElement = evt.srcElement;

    _altKey         = evt.altKey;
    _ctrlKey        = evt.ctrlKey;
    _metaKey        = evt.metaKey;
    _shiftKey       = evt.shiftKey;
    _buttonsPressed = evt.buttons;

    if (_engine->isPointerLock) {
      const auto offsetX = evt.movementX;
      const auto offsetY = evt.movementY;

      onTouch(std::nullopt, offsetX, offsetY);
      pointA = std::nullopt;
      pointB = std::nullopt;
    }
    else if (p->type == PointerEventTypes::POINTERDOWN && srcElement) {
      if (evt.srcElement) {
        evt.srcElement->setPointerCapture(evt.pointerId);
      }

      if (!pointA) {
        pointA = PointerTouch{
          evt.clientX,    // x
          evt.clientY,    // y
          evt.pointerId,  //  pointerId,
          evt.pointerType // pointerType
        };
      }
      else if (!pointB) {
        pointB = PointerTouch{
          evt.clientX,    // x
          evt.clientY,    // y
          evt.pointerId,  //  pointerId,
          evt.pointerType // pointerType
        };
      }

      onButtonDown(evt);

      if (!_noPreventDefault) {
        evt.preventDefault();
        if (_element)
          _element->focus();
      }
    }
    else if (p->type == PointerEventTypes::POINTERDOUBLETAP) {
      // onDoubleTap(evt.pointerType);
    }
    else if (p->type == PointerEventTypes::POINTERUP && srcElement) {
      if (evt.srcElement) {
        evt.srcElement->releasePointerCapture(evt.pointerId);
      }

      if (!isTouch) {
        pointB = std::nullopt; // Mouse and pen are mono pointer
      }

      // would be better to use pointers.remove(evt.pointerId) for multitouch
      // gestures, but emptying completely pointers collection is required to
      // fix a bug on iPhone : when changing orientation while pinching camera,
      // one pointer stay pressed forever if we don't release all pointers will
      // be ok to put back pointers.remove(evt.pointerId); when iPhone bug
      // corrected
      if (_engine->_badOS) {
        pointA = pointB = std::nullopt;
      }
      else {
        // only remove the impacted pointer in case of multitouch allowing on
        // most platforms switching from rotate to zoom and pan seamlessly.
        if (pointB && pointA && pointA->pointerId == evt.pointerId) {
          pointA = pointB;
          pointB = std::nullopt;
        }
        else if (pointA && pointB && pointB->pointerId == evt.pointerId) {
          pointB = std::nullopt;
        }
        else {
          pointA = pointB = std::nullopt;
        }
      }

      if (_previousPinchSquaredDistance != 0.f || _previousMultiTouchPanPosition) {
        // Previous pinch data is populated but a button has been lifted
        // so pinch has ended.
        onMultiTouch(pointA, pointB, _previousPinchSquaredDistance,
                     0.f, // pinchSquaredDistance
                     _previousMultiTouchPanPosition,
                     std::nullopt // multiTouchPanPosition
        );
        _previousPinchSquaredDistance  = 0.f;
        _previousMultiTouchPanPosition = std::nullopt;
      }

      onButtonUp(evt);

      if (!_noPreventDefault) {
        evt.preventDefault();
      }
    }
    else if (p->type == PointerEventTypes::POINTERMOVE) {
      if (!_noPreventDefault) {
        evt.preventDefault();
      }

      // One button down
      if (pointA && !pointB) {
        const auto offsetX = evt.clientX - pointA->x;
        const auto offsetY = evt.clientY - pointA->y;
        onTouch(pointA, offsetX, offsetY);

        pointA->x = evt.clientX;
        pointA->y = evt.clientY;
      }
      // Two buttons down: pinch
      else if (pointA && pointB) {
        auto ed                    = (pointA->pointerId == evt.pointerId) ? pointA : pointB;
        ed->x                      = evt.clientX;
        ed->y                      = evt.clientY;
        auto distX                 = pointA->x - pointB->x;
        auto distY                 = pointA->y - pointB->y;
        auto pinchSquaredDistance  = static_cast<float>((distX * distX) + (distY * distY));
        auto multiTouchPanPosition = PointerTouch{
          (pointA->x + pointB->x) / 2, // x
          (pointA->y + pointB->y) / 2, // y
          evt.pointerId,               //  pointerId,
          evt.pointerType              // pointerType
        };

        onMultiTouch(pointA, pointB, _previousPinchSquaredDistance, pinchSquaredDistance,
                     _previousMultiTouchPanPosition, multiTouchPanPosition);

        _previousMultiTouchPanPosition = multiTouchPanPosition;
        _previousPinchSquaredDistance  = pinchSquaredDistance;
      }
    }
  };

  _observer = ICameraInput<TCamera>::camera->getScene()->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN)
                     | static_cast<int>(PointerEventTypes::POINTERUP)
                     | static_cast<int>(PointerEventTypes::POINTERMOVE));

  _onLostFocus = [this](const FocusEvent&) {
    pointA = pointB                = std::nullopt;
    _previousPinchSquaredDistance  = 0;
    _previousMultiTouchPanPosition = std::nullopt;
    onLostFocus();
  };

  /*element.addEventListener("contextmenu",
      <EventListener>onContextMenu.bind(this), false);

  Tools.RegisterTopRootEvents([
      { name: "blur", handler: _onLostFocus }
  ]);*/
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::detachControl(ICanvas* /*ignored*/)
{
  if (_onLostFocus) {
    /* Tools::UnregisterTopRootEvents([
         { name: "blur", handler: _onLostFocus }
     ]);*/
  }

  if (_observer) {
    ICameraInput<TCamera>::camera->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;

    /*if (onContextMenu) {
        element->removeEventListener("contextmenu", onContextMenu);
    }*/

    _onLostFocus = nullptr;
  }

  _altKey         = false;
  _ctrlKey        = false;
  _metaKey        = false;
  _shiftKey       = false;
  _buttonsPressed = 0;
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::checkInputs()
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onDoubleTap(const std::string& /*type*/)
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onTouch(const std::optional<PointerTouch>& /*point*/,
                                               int /*offsetX*/, int /*offsetY*/)
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onMultiTouch(
  const std::optional<PointerTouch>& /*pointA*/, const std::optional<PointerTouch>& /*pointB*/,
  float /*previousPinchSquaredDistance*/, float /*pinchSquaredDistance*/,
  const std::optional<PointerTouch>& /*previousMultiTouchPanPosition*/,
  const std::optional<PointerTouch>& /*multiTouchPanPosition*/)
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onContextMenu(PointerEvent& evt)
{
  evt.preventDefault();
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onButtonDown(IPointerEvent& /*evt*/)
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onButtonUp(IPointerEvent& /*evt*/)
{
}

template <class TCamera>
void BaseCameraPointersInput<TCamera>::onLostFocus()
{
}

// Explicit template instantiation
template class BaseCameraPointersInput<ArcRotateCamera>;
template class BaseCameraPointersInput<FollowCamera>;

} // end of namespace BABYLON
