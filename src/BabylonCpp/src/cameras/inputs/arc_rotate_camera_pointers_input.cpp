#include <babylon/cameras/inputs/arc_rotate_camera_pointers_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

ArcRotateCameraPointersInput::ArcRotateCameraPointersInput()
    : angularSensibilityX{1000.f}
    , angularSensibilityY{1000.f}
    , pinchPrecision{12.f}
    , pinchDeltaPercentage{0.f}
    , useNaturalPinchZoom{false}
    , pinchZoom{true}
    , panningSensibility{1000.f}
    , multiTouchPanning{true}
    , multiTouchPanAndZoom{true}
    , pinchInwards{true}
    , _isPanClick{false}
    , _twoFingerActivityCount{0}
    , _isPinching{false}
{
}

ArcRotateCameraPointersInput::~ArcRotateCameraPointersInput() = default;

std::string ArcRotateCameraPointersInput::getClassName() const
{
  return "ArcRotateCameraPointersInput";
}

void ArcRotateCameraPointersInput::_computeMultiTouchPanning(
  const std::optional<PointerTouch>& previousMultiTouchPanPosition,
  const std::optional<PointerTouch>& multiTouchPanPosition)
{
  if (panningSensibility != 0.f && previousMultiTouchPanPosition && multiTouchPanPosition) {
    const auto moveDeltaX = multiTouchPanPosition->x - previousMultiTouchPanPosition->x;
    const auto moveDeltaY = multiTouchPanPosition->y - previousMultiTouchPanPosition->y;
    camera->inertialPanningX += -moveDeltaX / panningSensibility;
    camera->inertialPanningY += moveDeltaY / panningSensibility;
  }
}

void ArcRotateCameraPointersInput::_computePinchZoom(float previousPinchSquaredDistance,
                                                     float pinchSquaredDistance)
{
  if (useNaturalPinchZoom) {
    camera->radius
      = camera->radius * std::sqrt(previousPinchSquaredDistance) / std::sqrt(pinchSquaredDistance);
  }
  else if (pinchDeltaPercentage) {
    camera->inertialRadiusOffset += (pinchSquaredDistance - previousPinchSquaredDistance) * 0.001f
                                    * camera->radius * pinchDeltaPercentage;
  }
  else {
    camera->inertialRadiusOffset += (pinchSquaredDistance - previousPinchSquaredDistance)
                                    / (pinchPrecision * (pinchInwards ? 1.f : -1.f)
                                       * (angularSensibilityX + angularSensibilityY) / 2.f);
  }
}

void ArcRotateCameraPointersInput::onTouch(const std::optional<PointerTouch>& /*point*/,
                                           int offsetX, int offsetY)
{
  if (panningSensibility != 0.f && ((_ctrlKey && camera->_useCtrlForPanning) || _isPanClick)) {
    camera->inertialPanningX += -offsetX / panningSensibility;
    camera->inertialPanningY += offsetY / panningSensibility;
  }
  else {
    camera->inertialAlphaOffset -= offsetX / angularSensibilityX;
    camera->inertialBetaOffset -= offsetY / angularSensibilityY;
  }
}

void ArcRotateCameraPointersInput::onDoubleTap(const std::string& /*type*/)
{
  if (camera->useInputToRestoreState) {
    camera->restoreState();
  }
}

void ArcRotateCameraPointersInput::onMultiTouch(
  const std::optional<PointerTouch>& /*pointA*/, const std::optional<PointerTouch>& /*pointB*/,
  float previousPinchSquaredDistance, float pinchSquaredDistance,
  const std::optional<PointerTouch>& previousMultiTouchPanPosition,
  const std::optional<PointerTouch>& multiTouchPanPosition)
{
  if (previousPinchSquaredDistance == 0.f && !previousMultiTouchPanPosition.has_value()) {
    // First time this method is called for new pinch.
    // Next time this is called there will be a
    // previousPinchSquaredDistance and pinchSquaredDistance to compare.
    return;
  }
  if (pinchSquaredDistance == 0.f && !multiTouchPanPosition.has_value()) {
    // Last time this method is called at the end of a pinch.
    return;
  }

  // Zoom and panning enabled together
  if (multiTouchPanAndZoom) {
    _computePinchZoom(previousPinchSquaredDistance, pinchSquaredDistance);
    _computeMultiTouchPanning(previousMultiTouchPanPosition, multiTouchPanPosition);
  }
  // Zoom and panning enabled but only one at a time
  else if (multiTouchPanning && pinchZoom) {
    _twoFingerActivityCount++;

    if (_isPinching
        || (_twoFingerActivityCount < 20
            && std::abs(std::sqrt(pinchSquaredDistance) - std::sqrt(previousPinchSquaredDistance))
                 > camera->pinchToPanMaxDistance)) {

      // Since pinch has not been active long, assume we intend to zoom.
      _computePinchZoom(previousPinchSquaredDistance, pinchSquaredDistance);

      // Since we are pinching, remain pinching on next iteration.
      _isPinching = true;
    }
    else {
      // Pause between pinch starting and moving implies not a zoom event. Pan instead.
      _computeMultiTouchPanning(previousMultiTouchPanPosition, multiTouchPanPosition);
    }
  }
  // Panning enabled, zoom disabled
  else if (multiTouchPanning) {
    _computeMultiTouchPanning(previousMultiTouchPanPosition, multiTouchPanPosition);
  }
  // Zoom enabled, panning disabled
  else if (pinchZoom) {
    _computePinchZoom(previousPinchSquaredDistance, pinchSquaredDistance);
  }
}

void ArcRotateCameraPointersInput::onButtonDown(PointerEvent& evt)
{
  _isPanClick = evt.button == camera->_panningMouseButton;
}

void ArcRotateCameraPointersInput::onButtonUp(PointerEvent& /*evt*/)
{
  _twoFingerActivityCount = 0;
  _isPinching             = false;
}

void ArcRotateCameraPointersInput::onLostFocus()
{
  _isPanClick             = false;
  _twoFingerActivityCount = 0;
  _isPinching             = false;
}

} // end of namespace BABYLON
