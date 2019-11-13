#include <babylon/cameras/inputs/follow_camera_mouse_wheel_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

FollowCameraMouseWheelInput::FollowCameraMouseWheelInput()
    : axisControlRadius{true}
    , axisControlHeight{false}
    , axisControlRotation{false}
    , wheelPrecision{3.f}
    , wheelDeltaPercentage{0.f}
    , _canvas{nullptr}
    , _noPreventDefault{false}
    , _wheel{nullptr}
    , _observer{nullptr}
{
}

FollowCameraMouseWheelInput::~FollowCameraMouseWheelInput() = default;

void FollowCameraMouseWheelInput::attachControl(ICanvas* canvas,
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

    // Chrome, Safari: event.deltaY
    // IE: event.wheelDelta
    // Firefox: event.detail (inverted)
    auto wheelDelta = std::max(-1.f, std::min(1.f, event.wheelDelta));
    if (wheelDeltaPercentage != 0.f) {
      if (axisControlRadius) {
        delta = wheelDelta * 0.01f * wheelDeltaPercentage * camera->radius;
      }
      else if (axisControlHeight) {
        delta
          = wheelDelta * 0.01f * wheelDeltaPercentage * camera->heightOffset;
      }
      else if (axisControlRotation) {
        delta
          = wheelDelta * 0.01f * wheelDeltaPercentage * camera->rotationOffset;
      }
    }
    else {
      delta = wheelDelta * wheelPrecision;
    }

    if (delta != 0.f) {
      if (axisControlRadius) {
        camera->radius += delta;
      }
      else if (axisControlHeight) {
        camera->heightOffset -= delta;
      }
      else if (axisControlRotation) {
        camera->rotationOffset -= delta;
      }
    }

    if (!_noPreventDefault) {
      event.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _wheel, static_cast<int>(PointerEventTypes::POINTERWHEEL));
}

void FollowCameraMouseWheelInput::detachControl(ICanvas* canvas)
{
  if (_observer && canvas) {
    camera->getScene()->onPointerObservable.remove(_observer);
    _observer = nullptr;
    _wheel    = nullptr;
  }
}

void FollowCameraMouseWheelInput::checkInputs()
{
}

const std::string FollowCameraMouseWheelInput::getClassName() const
{
  return "ArcRotateCameraMouseWheelInput";
}

const std::string FollowCameraMouseWheelInput::getSimpleName() const
{
  return "mousewheel";
}

} // end of namespace BABYLON
