#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>
#include <babylon/events/pointer_event_types.h>

namespace BABYLON {

ArcRotateCameraMouseWheelInput::ArcRotateCameraMouseWheelInput()
    : wheelPrecision{3.f}
    , wheelDeltaPercentage{0.f}
    , _canvas{nullptr}
    , _noPreventDefault{false}
    , _wheel{nullptr}
    , _observer{nullptr}
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

  _wheel = [this](PointerInfo* p, EventState&) {
    // sanity check - this should be a PointerWheel event.
    if (p->type != PointerEventTypes::POINTERWHEEL) {
      return;
    }
    const auto& event = p->mouseWheelEvent;
    float delta       = 0.f;
    if (!stl_util::almost_equal(event.wheelDelta, 0.f)) {
      if (!stl_util::almost_equal(wheelDeltaPercentage, 0.f)) {
        const auto wheelDelta
          = (event.wheelDelta * 0.01f * wheelDeltaPercentage) * camera->radius;
        if (event.wheelDelta > 0.f) {
          delta = wheelDelta / (1.f + wheelDeltaPercentage);
        }
        else {
          delta = wheelDelta * (1.f + wheelDeltaPercentage);
        }
      }
      else {
        delta = event.wheelDelta / (wheelPrecision * 40.f);
      }
    }
    else if (!stl_util::almost_equal(event.detail, 0.f)) {
      delta = -event.detail / wheelPrecision;
    }

    if (!stl_util::almost_equal(delta, 0.f)) {
      camera->inertialRadiusOffset += delta;
    }

    if (!_noPreventDefault) {
      event.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _wheel, static_cast<int>(PointerEventTypes::POINTERWHEEL));
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

const char* ArcRotateCameraMouseWheelInput::getClassName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

const char* ArcRotateCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
