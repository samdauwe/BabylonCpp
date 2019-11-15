#include <babylon/cameras/inputs/follow_camera_pointers_input.h>

#include <babylon/cameras/follow_camera.h>
#include <babylon/core/logging.h>
#include <sstream>

namespace BABYLON {

FollowCameraPointersInput::FollowCameraPointersInput()
    : angularSensibilityX{1.f}
    , angularSensibilityY{1.f}
    , pinchPrecision{10000.f}
    , pinchDeltaPercentage{0.f}
    , axisXControlRadius{false}
    , axisXControlHeight{false}
    , axisXControlRotation{true}
    , axisYControlRadius{false}
    , axisYControlHeight{true}
    , axisYControlRotation{false}
    , axisPinchControlRadius{true}
    , axisPinchControlHeight{false}
    , axisPinchControlRotation{false}
    , warningEnable{true}
    , _warningCounter{0}
{
}

FollowCameraPointersInput::~FollowCameraPointersInput() = default;

const std::string FollowCameraPointersInput::getClassName() const
{
  return "FollowCameraPointersInput";
}

void FollowCameraPointersInput::onTouch(
  const std::optional<PointerTouch>& /*pointA*/, int offsetX, int offsetY)
{
  _warning();

  if (axisXControlRotation) {
    camera->rotationOffset += offsetX / angularSensibilityX;
  }
  else if (axisYControlRotation) {
    camera->rotationOffset += offsetY / angularSensibilityX;
  }

  if (axisXControlHeight) {
    camera->heightOffset += offsetX / angularSensibilityY;
  }
  else if (axisYControlHeight) {
    camera->heightOffset += offsetY / angularSensibilityY;
  }

  if (axisXControlRadius) {
    camera->radius -= offsetX / angularSensibilityY;
  }
  else if (axisYControlRadius) {
    camera->radius -= offsetY / angularSensibilityY;
  }
}

void FollowCameraPointersInput::onMultiTouch(
  const std::optional<PointerTouch>& /*pointA*/,
  const std::optional<PointerTouch>& /*pointB*/,
  float previousPinchSquaredDistance, float pinchSquaredDistance,
  const std::optional<PointerTouch>& previousMultiTouchPanPosition,
  const std::optional<PointerTouch>& multiTouchPanPosition)
{
  if (previousPinchSquaredDistance == 0.f
      && !previousMultiTouchPanPosition.has_value()) {
    // First time this method is called for new pinch.
    // Next time this is called there will be a
    // previousPinchSquaredDistance and pinchSquaredDistance to compare.
    return;
  }
  if (pinchSquaredDistance == 0.f && !multiTouchPanPosition.has_value()) {
    // Last time this method is called at the end of a pinch.
    return;
  }
  auto pinchDelta
    = (pinchSquaredDistance - previousPinchSquaredDistance)
      / (pinchPrecision * (angularSensibilityX + angularSensibilityY) / 2.f);

  if (pinchDeltaPercentage != 0.f) {
    pinchDelta *= 0.01f * pinchDeltaPercentage;
    if (axisPinchControlRotation) {
      camera->rotationOffset += pinchDelta * camera->rotationOffset;
    }
    if (axisPinchControlHeight) {
      camera->heightOffset += pinchDelta * camera->heightOffset;
    }
    if (axisPinchControlRadius) {
      camera->radius -= pinchDelta * camera->radius;
    }
  }
  else {
    if (axisPinchControlRotation) {
      camera->rotationOffset += pinchDelta;
    }

    if (axisPinchControlHeight) {
      camera->heightOffset += pinchDelta;
    }

    if (axisPinchControlRadius) {
      camera->radius -= pinchDelta;
    }
  }
}

void FollowCameraPointersInput::_warning()
{
  if (!warningEnable || _warningCounter++ % 100 != 0) {
    return;
  }
  std::ostringstream warn;
  warn << "It probably only makes sense to control ONE camera property with "
          "each pointer axis. Set 'warningEnable = false' if you are sure. "
          "Currently enabled: ";

  if ((axisXControlRotation + axisXControlHeight + axisXControlRadius) > 1.f) {
    warn << "axisXControlRotation: " << axisXControlRotation
         << ", axisXControlHeight: " << axisXControlHeight
         << ", axisXControlRadius: " << axisXControlRadius;
  }

  if ((axisYControlRotation + axisYControlHeight + axisYControlHeight) > 1.f) {
    warn << "axisYControlRotation: " << axisYControlRotation
         << ", axisYControlHeight: " << axisYControlHeight
         << ", axisYControlRadius: " << axisYControlRadius;
  }

  if ((axisPinchControlRotation + axisPinchControlHeight
       + axisPinchControlHeight)
      > 1.f) {
    warn << "axisPinchControlRotation: " << axisPinchControlRotation
         << ", axisPinchControlHeight: " << axisPinchControlHeight
         << ", axisPinchControlRadius: " << axisPinchControlRadius;
  }

  BABYLON_LOGF_WARN("FollowCameraPointersInput", "%s", warn.str().c_str())
}

} // end of namespace BABYLON
