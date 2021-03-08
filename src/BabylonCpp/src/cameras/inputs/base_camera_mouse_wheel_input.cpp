#include <babylon/cameras/inputs/base_camera_mouse_wheel_input.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

BaseCameraMouseWheelInput::BaseCameraMouseWheelInput()
    : wheelPrecisionX{3.f}
    , wheelPrecisionY{3.f}
    , wheelPrecisionZ{3.f}
    , _wheelDeltaX{0.f}
    , _wheelDeltaY{0.f}
    , _wheelDeltaZ{0.f}
    , _wheel{nullptr}
    , _observer{nullptr}
    , _ffMultiplier{12.f}
    , _normalize{120.f}
{
}

BaseCameraMouseWheelInput::~BaseCameraMouseWheelInput() = default;

void BaseCameraMouseWheelInput::attachControl(bool noPreventDefault)
{
  _wheel = [this, noPreventDefault](PointerInfo* pointer, EventState& /*es*/) -> void {
    // sanity check - this should be a PointerWheel event.
    if (pointer->type != PointerEventTypes::POINTERWHEEL) {
      return;
    }

    const auto& event = pointer->mouseWheelEvent;

    // If this happens to be set to DOM_DELTA_LINE, adjust accordingly
    const auto platformScale
      = event.deltaMode == EventConstants::DOM_DELTA_LINE ? _ffMultiplier : 1.f;

    if (event.deltaY != 0.f) {
      // Most recent browsers versions have delta properties.
      // Firefox >= v17  (Has WebGL >= v4)
      // Chrome >=  v31  (Has WebGL >= v8)
      // Edge >=    v12  (Has WebGl >= v12)
      // https://developer.mozilla.org/en-US/docs/Web/API/WheelEvent
      _wheelDeltaX += (wheelPrecisionX * platformScale * event.deltaX) / _normalize;
      _wheelDeltaY -= (wheelPrecisionY * platformScale * event.deltaY) / _normalize;
      _wheelDeltaZ += (wheelPrecisionZ * platformScale * event.deltaZ) / _normalize;
    }
#if 0
    else if (event.wheelDeltaY != 0.f) {
      // Unsure whether these catch anything more. Documentation
      // online is contradictory.
      _wheelDeltaX += (wheelPrecisionX * platformScale * event.wheelDeltaX) / _normalize;
      _wheelDeltaY -= (wheelPrecisionY * platformScale * event.wheelDeltaY) / _normalize;
      _wheelDeltaZ += (wheelPrecisionZ * platformScale * event.wheelDeltaZ) / _normalize;
    }
    else if (event.wheelDelta) {
      // IE >= v9   (Has WebGL >= v11)
      // Maybe others?
      _wheelDeltaY -= (wheelPrecisionY * event.wheelDelta) / _normalize;
    }
#endif

    /* if (event.preventDefault) */ {
      if (!noPreventDefault) {
        event.preventDefault();
      }
    }
  };

  _observer = get_camera()->getScene()->onPointerObservable.add(
    _wheel, static_cast<int>(PointerEventTypes::POINTERWHEEL));
}

void BaseCameraMouseWheelInput::detachControl(ICanvas* /*ignored*/)
{
  if (_observer) {
    get_camera()->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;
    _wheel    = nullptr;
  }
  if (onChangedObservable) {
    onChangedObservable.clear();
  }
}

void BaseCameraMouseWheelInput::checkInputs()
{
  _tmpMouseWheelChangeEvent.wheelDeltaX = _wheelDeltaX;
  _tmpMouseWheelChangeEvent.wheelDeltaY = _wheelDeltaY;
  _tmpMouseWheelChangeEvent.wheelDeltaZ = _wheelDeltaZ;

  onChangedObservable.notifyObservers(&_tmpMouseWheelChangeEvent);

  // Clear deltas.
  _wheelDeltaX = 0.f;
  _wheelDeltaY = 0.f;
  _wheelDeltaZ = 0.f;
}

std::string BaseCameraMouseWheelInput::getClassName() const
{
  return "BaseCameraMouseWheelInput";
}

std::string BaseCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
