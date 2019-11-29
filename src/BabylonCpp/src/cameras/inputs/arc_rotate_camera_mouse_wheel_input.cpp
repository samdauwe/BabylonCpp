#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/maths/scalar.h>

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

ArcRotateCameraMouseWheelInput::~ArcRotateCameraMouseWheelInput() = default;

float ArcRotateCameraMouseWheelInput::computeDeltaFromMouseWheelLegacyEvent(
  const MouseWheelEvent& mouseWheelLegacyEvent, float radius) const
{
  auto delta = 0.f;
  const auto wheelDelta
    = (mouseWheelLegacyEvent.wheelDelta * 0.01f * wheelDeltaPercentage)
      * radius;
  if (mouseWheelLegacyEvent.wheelDelta > 0.f) {
    delta = wheelDelta / (1.f + wheelDeltaPercentage);
  }
  else {
    delta = wheelDelta * (1.f + wheelDeltaPercentage);
  }
  return delta;
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
    auto delta        = 0.f;

    const auto& mouseWheelLegacyEvent = event;
    if (!stl_util::almost_equal(mouseWheelLegacyEvent.wheelDelta, 0.f)) {
      if (!stl_util::almost_equal(wheelDeltaPercentage, 0.f)) {
        delta = computeDeltaFromMouseWheelLegacyEvent(mouseWheelLegacyEvent,
                                                      camera->radius);

        // If zooming in, estimate the target radius and use that to compute the
        // delta for inertia this will stop multiple scroll events zooming in
        // from adding too much inertia
        if (delta > 0.f) {
          auto estimatedTargetRadius = camera->radius;
          auto targetInertia         = camera->inertialRadiusOffset + delta;
          for (uint32_t i = 0; i < 20 && std::abs(targetInertia) > 0.001f;
               ++i) {
            estimatedTargetRadius -= targetInertia;
            targetInertia *= camera->inertia;
          }
          estimatedTargetRadius = Scalar::Clamp(
            estimatedTargetRadius, 0.f, std::numeric_limits<float>::max());
          delta = computeDeltaFromMouseWheelLegacyEvent(mouseWheelLegacyEvent,
                                                        estimatedTargetRadius);
        }
      }
      else {
        delta = mouseWheelLegacyEvent.wheelDelta / (wheelPrecision * 40.f);
      }
    }
    else {
      const auto deltaValue = event.detail;
      delta                 = -deltaValue / wheelPrecision;
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

std::string ArcRotateCameraMouseWheelInput::getClassName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

std::string ArcRotateCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
