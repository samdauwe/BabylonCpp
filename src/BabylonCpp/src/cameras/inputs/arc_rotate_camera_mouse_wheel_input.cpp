#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

ArcRotateCameraMouseWheelInput::ArcRotateCameraMouseWheelInput()
    : wheelPrecision{3.f}
    , wheelDeltaPercentage{0.f}
    , _noPreventDefault{false}
    , _wheel{nullptr}
    , _observer{nullptr}
{
}

ArcRotateCameraMouseWheelInput::~ArcRotateCameraMouseWheelInput() = default;

void ArcRotateCameraMouseWheelInput::attachControl(bool noPreventDefault)
{
  _noPreventDefault = noPreventDefault;

  _wheel = [this](PointerInfo* p, EventState&) {
    // sanity check - this should be a PointerWheel event.
    if (p->type != PointerEventTypes::POINTERWHEEL) {
      return;
    }
    const auto& event = p->mouseWheelEvent;
    auto delta        = 0.f;
    auto wheelDelta   = 0.f;

    // Using the inertia percentage, calculate the value needed to move based on set wheel delta
    // percentage
    if (wheelDeltaPercentage != 0.f) {
      wheelDelta = (event.deltaY > 0 ? -1.f : 1.f) * camera->radius * wheelDeltaPercentage;
      delta      = wheelDelta * (camera->inertia == 1.f ? 1.f : (1.f - camera->inertia));
    }
    // If there's no percentage set, just handle using a value that emulates Chrome
    else {
      wheelDelta = (event.deltaY > 0 ? -100.f : 100.f);

      if (wheelPrecision != 0.f) {
        delta = wheelDelta / (wheelPrecision * 40.f);
      }
    }

    if (delta) {
      camera->inertialRadiusOffset += delta;
    }

    if (event.preventDefault) {
      if (!_noPreventDefault) {
        event.preventDefault();
      }
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _wheel, static_cast<int>(PointerEventTypes::POINTERWHEEL));
}

void ArcRotateCameraMouseWheelInput::detachControl(ICanvas* /*ignored*/)
{
  if (_observer) {
    camera->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;
    _wheel    = nullptr;
  }
}

void ArcRotateCameraMouseWheelInput::checkInputs()
{
}

std::string ArcRotateCameraMouseWheelInput::getClassName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

std::string ArcRotateCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
