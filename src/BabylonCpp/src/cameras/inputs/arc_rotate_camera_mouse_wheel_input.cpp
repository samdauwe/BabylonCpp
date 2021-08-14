#include <babylon/cameras/inputs/arc_rotate_camera_mouse_wheel_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

const float ArcRotateCameraMouseWheelInput::ffMultiplier = 40.f;

ArcRotateCameraMouseWheelInput::ArcRotateCameraMouseWheelInput()
    : wheelPrecision{3.f}
    , zoomToMouseLocation{false}
    , wheelDeltaPercentage{0.f}
    , customComputeDeltaFromMouseWheel{nullptr}
    , _noPreventDefault{false}
    , _wheel{nullptr}
    , _observer{nullptr}
    , _hitPlane{std::nullopt}
    , _inertialPanning{Vector3::Zero()}
{
}

ArcRotateCameraMouseWheelInput::~ArcRotateCameraMouseWheelInput() = default;

float ArcRotateCameraMouseWheelInput::computeDeltaFromMouseWheelLegacyEvent(float mouseWheelDelta,
                                                                            float radius)
{
  auto delta            = 0.f;
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

    if (customComputeDeltaFromMouseWheel) {
      delta = customComputeDeltaFromMouseWheel(wheelDelta, this, event);
    }
    else {
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
        delta = wheelDelta / (wheelPrecision * 40.f);
      }
    }

    if (delta) {
      if (zoomToMouseLocation && _hitPlane) {
        _zoomToMouse(delta);
      }
      else {
        camera->inertialRadiusOffset += delta;
      }
    }

    if (event.preventDefault) {
      if (!_noPreventDefault) {
        event.preventDefault();
      }
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _wheel, static_cast<int>(PointerEventTypes::POINTERWHEEL));

  if (zoomToMouseLocation) {
    _inertialPanning.setAll(0.f);
  }
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
  if (!zoomToMouseLocation) {
    return;
  }

  const auto motion
    = 0.f + camera->inertialAlphaOffset + camera->inertialBetaOffset + camera->inertialRadiusOffset;
  if (motion) {
    // if zooming is still happening as a result of inertia, then we also need to update
    // the hit plane.
    _updateHitPlane();

    // Note we cannot  use arcRotateCamera.inertialPlanning here because arcRotateCamera panning
    // uses a different panningInertia which could cause this panning to get out of sync with
    // the zooming, and for this to work they must be exactly in sync.
    camera->target().addInPlace(_inertialPanning);
    _inertialPanning.scaleInPlace(camera->inertia);
    _zeroIfClose(_inertialPanning);
  }
}

std::string ArcRotateCameraMouseWheelInput::getClassName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

std::string ArcRotateCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

void ArcRotateCameraMouseWheelInput::_updateHitPlane()
{
  const auto direction = camera->target().subtract(camera->position);
  _hitPlane            = Plane::FromPositionAndNormal(Vector3::Zero(), direction);
}

Vector3 ArcRotateCameraMouseWheelInput::_getPosition() const
{
  const auto scene = camera->getScene();

  // since the _hitPlane is always updated to be orthogonal to the camera position vector
  // we don't have to worry about this ray shooting off to infinity. This ray creates
  // a vector defining where we want to zoom to.
  auto identityMatrix = Matrix::Identity();
  auto ray
    = scene->createPickingRay(scene->pointerX, scene->pointerY, identityMatrix, camera, false);
  auto distance = 0.f;
  if (_hitPlane) {
    distance = ray.intersectsPlane(*_hitPlane).value_or(0.f);
  }

  // not using this ray again, so modifying its vectors here is fine
  return ray.origin.addInPlace(ray.direction.scaleInPlace(distance));
}

void ArcRotateCameraMouseWheelInput::_zoomToMouse(float delta)
{
  const auto inertiaComp = 1.f - camera->inertia;
  if (camera->lowerRadiusLimit) {
    const auto lowerLimit = camera->lowerRadiusLimit.value_or(0.f);
    if (camera->radius - (camera->inertialRadiusOffset + delta) / inertiaComp < lowerLimit) {
      delta = (camera->radius - lowerLimit) * inertiaComp - camera->inertialRadiusOffset;
    }
  }
  if (camera->upperRadiusLimit) {
    const auto upperLimit = camera->upperRadiusLimit.value_or(0.f);
    if (camera->radius - (camera->inertialRadiusOffset + delta) / inertiaComp > upperLimit) {
      delta = (camera->radius - upperLimit) * inertiaComp - camera->inertialRadiusOffset;
    }
  }

  const auto zoomDistance = delta / inertiaComp;
  const auto ratio        = zoomDistance / camera->radius;
  const auto vec          = _getPosition();

  // Now this vector tells us how much we also need to pan the camera
  // so the targeted mouse location becomes the center of zooming.
  const auto directionToZoomLocation = vec.subtract(camera->target);
  auto offset                        = directionToZoomLocation.scale(ratio);
  offset.scaleInPlace(inertiaComp);
  _inertialPanning.addInPlace(offset);

  camera->inertialRadiusOffset += delta;
}

void ArcRotateCameraMouseWheelInput::_zeroIfClose(Vector3& vec)
{
  if (std::abs(vec.x) < Math::Epsilon) {
    vec.x = 0.f;
  }
  if (std::abs(vec.y) < Math::Epsilon) {
    vec.y = 0.f;
  }
  if (std::abs(vec.z) < Math::Epsilon) {
    vec.z = 0.f;
  }
}

} // end of namespace BABYLON
