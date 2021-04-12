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

float ArcRotateCameraMouseWheelInput::computeDeltaFromMouseWheelLegacyEvent(float mouseWheelDelta,
                                                                            float radius)
{
  float delta           = 0.f;
  const auto wheelDelta = (mouseWheelDelta * 0.01f * wheelDeltaPercentage) * radius;
  if (mouseWheelDelta > 0.f) {
    delta = wheelDelta / (1.f + wheelDeltaPercentage);
  }
  else {
    delta = wheelDelta * (1.f + wheelDeltaPercentage);
  }
  return delta;
}

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

    if (event.wheelDelta) {
      wheelDelta = *event.wheelDelta;
    }
    else {
      wheelDelta = -(event.deltaY || event.detail) * 60.f;
    }

    if (wheelDeltaPercentage) {
      delta = computeDeltaFromMouseWheelLegacyEvent(wheelDelta, camera->radius);

      // If zooming in, estimate the target radius and use that to compute the delta for inertia
      // this will stop multiple scroll events zooming in from adding too much inertia
      if (delta > 0.f) {
        auto estimatedTargetRadius = camera->radius;
        auto targetInertia         = camera->inertialRadiusOffset + delta;
        for (unsigned int i = 0; i < 20 && std::abs(targetInertia) > 0.001f; ++i) {
          estimatedTargetRadius -= targetInertia;
          targetInertia *= camera->inertia;
        }
        estimatedTargetRadius
          = Scalar::Clamp(estimatedTargetRadius, 0.f, std::numeric_limits<float>::max());
        delta = computeDeltaFromMouseWheelLegacyEvent(wheelDelta, estimatedTargetRadius);
      }
    }
    else {
      delta = wheelDelta / (wheelPrecision * 40);
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
