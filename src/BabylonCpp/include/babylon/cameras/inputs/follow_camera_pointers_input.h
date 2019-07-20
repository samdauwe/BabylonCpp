#ifndef BABYLON_CAMERAS_INPUTS_FOLLOW_CAMERA_POINTERS_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FOLLOW_CAMERA_POINTERS_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/inputs/base_camera_pointers_input.h>

namespace BABYLON {

/**
 * @brief Manage the pointers inputs to control an follow camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FollowCameraPointersInput
    : public BaseCameraPointersInput<FollowCamera> {

public:
  FollowCameraPointersInput();
  ~FollowCameraPointersInput() override;

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Called on pointer POINTERMOVE event if only a single touch is
   * active. Override this method to provide functionality.
   */
  void onTouch(const std::optional<PointerTouch>& pointA, int offsetX,
               int offsetY) override;

  /**
   * @brief Called on pointer POINTERMOVE event if multiple touches are active.
   * Override this method to provide functionality.
   */
  void onMultiTouch(
    const std::optional<PointerTouch>& pointA,
    const std::optional<PointerTouch>& pointB,
    float previousPinchSquaredDistance, float pinchSquaredDistance,
    const std::optional<PointerTouch>& previousMultiTouchPanPosition,
    const std::optional<PointerTouch>& multiTouchPanPosition) override;

private:
  void _warning();

public:
  /**
   * Defines the pointer angular sensibility along the X axis or how fast is
   * the camera rotating.
   * A negative number will reverse the axis direction.
   */
  float angularSensibilityX;

  /**
   * Defines the pointer angular sensibility along the Y axis or how fast is
   * the camera rotating.
   * A negative number will reverse the axis direction.
   */
  float angularSensibilityY;

  /**
   * Defines the pointer pinch precision or how fast is the camera zooming.
   * A negative number will reverse the axis direction.
   */
  float pinchPrecision;

  /**
   * pinchDeltaPercentage will be used instead of pinchPrecision if different
   * from 0.
   * It defines the percentage of current camera.radius to use as delta when
   * pinch zoom is used.
   */
  float pinchDeltaPercentage;

  /**
   * Pointer X axis controls zoom. (X axis modifies camera.radius value.)
   */
  bool axisXControlRadius;

  /**
   * Pointer X axis controls height. (X axis modifies camera.heightOffset
   * value.)
   */
  bool axisXControlHeight;

  /**
   * Pointer X axis controls angle. (X axis modifies camera.rotationOffset
   * value.)
   */
  bool axisXControlRotation;

  /**
   * Pointer Y axis controls zoom. (Y axis modifies camera.radius value.)
   */
  bool axisYControlRadius;

  /**
   * Pointer Y axis controls height. (Y axis modifies camera.heightOffset
   * value.)
   */
  bool axisYControlHeight;

  /**
   * Pointer Y axis controls angle. (Y axis modifies camera.rotationOffset
   * value.)
   */
  bool axisYControlRotation;

  /**
   * Pinch controls zoom. (Pinch modifies camera.radius value.)
   */
  bool axisPinchControlRadius;

  /**
   * Pinch controls height. (Pinch modifies camera.heightOffset value.)
   */
  bool axisPinchControlHeight;

  /**
   * Pinch controls angle. (Pinch modifies camera.rotationOffset value.)
   */
  bool axisPinchControlRotation;

  /**
   * Log error messages if basic misconfiguration has occurred.
   */
  bool warningEnable;

private:
  /* Check for obvious misconfiguration. */
  size_t _warningCounter;

}; // end of class FollowCameraPointersInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FOLLOW_CAMERA_POINTERS_INPUT_H
