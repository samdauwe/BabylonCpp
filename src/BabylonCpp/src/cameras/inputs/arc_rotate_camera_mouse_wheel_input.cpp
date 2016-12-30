#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>

#include <babylon/engine/pointer_event_types.h>

namespace BABYLON {

ArcRotateCameraMouseWheelInput::ArcRotateCameraMouseWheelInput()
    : wheelPrecision{3.f}, _canvas{nullptr}, _noPreventDefault{false}
{
}

ArcRotateCameraMouseWheelInput::~ArcRotateCameraMouseWheelInput()
{
}

void ArcRotateCameraMouseWheelInput::attachControl(ICanvas* canvas,
                                                   bool noPreventDefault)
{
  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  _wheel = [this](const PointerInfo& p, const EventState& /*s*/) {
    // sanity check - this should be a PointerWheel event.
    if (p.type != PointerEventTypes::POINTERWHEEL) {
      return;
    }
    const auto& event = p.mouseWheelEvent;
    float delta       = 0.f;
    if (!std_util::almost_equal(event.wheelDelta, 0.f)) {
      delta = event.wheelDelta / (wheelPrecision * 40.f);
    }
    else if (!std_util::almost_equal(event.detail, 0.f)) {
      delta = -event.detail / wheelPrecision;
    }

    if (!std_util::almost_equal(delta, 0.f)) {
      camera->inertialRadiusOffset += delta;
    }

    if (!_noPreventDefault) {
      event.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _wheel, PointerEventTypes::POINTERWHEEL);
}

void ArcRotateCameraMouseWheelInput::detachControl(ICanvas* canvas)
{
  if (_observer && canvas) {
    camera->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;
    _wheel    = nullptr;
  }
}

void ArcRotateCameraMouseWheelInput::checkInputs()
{
}

const char* ArcRotateCameraMouseWheelInput::getTypeName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

const char* ArcRotateCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
