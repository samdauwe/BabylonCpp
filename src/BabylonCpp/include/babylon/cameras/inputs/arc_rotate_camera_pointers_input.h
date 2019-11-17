#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/inputs/base_camera_pointers_input.h>

namespace BABYLON {

/**
 * @brief Manage the pointers inputs to control an arc rotate camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraPointersInput
    : public BaseCameraPointersInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraPointersInput();
  ~ArcRotateCameraPointersInput() override; // = default

  /**
   * @brief Gets the class name of the current intput.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Called on pointer POINTERMOVE event if only a single touch is
   * active. Override this method to provide functionality.
   */
  void onTouch(const std::optional<PointerTouch>& point, int offsetX, int offsetY) override;

  /**
   * @brief Called on pointer POINTERDOUBLETAP event.
   * Override this method to provide functionality on POINTERDOUBLETAP event.
   */
  void onDoubleTap(const std::string& type) override;

  /**
   * @brief Called on pointer POINTERMOVE event if multiple touches are active.
   * Override this method to provide functionality.
   */
  void onMultiTouch(const std::optional<PointerTouch>& pointA,
                    const std::optional<PointerTouch>& pointB, float previousPinchSquaredDistance,
                    float pinchSquaredDistance,
                    const std::optional<PointerTouch>& previousMultiTouchPanPosition,
                    const std::optional<PointerTouch>& multiTouchPanPosition) override;

  /**
   * @brief Called each time a new POINTERDOWN event occurs. Ie, for each button
   * press.
   * Override this method to provide functionality.
   */
  void onButtonDown(PointerEvent& evt) override;

  /**
   * @brief Called each time a new POINTERUP event occurs. Ie, for each button
   * release.
   * Override this method to provide functionality.
   */
  void onButtonUp(PointerEvent& evt) override;

  /**
   * @brief Called when window becomes inactive.
   * Override this method to provide functionality.
   */
  void onLostFocus() override;

public:
  /**
   * Defines the pointer angular sensibility  along the X axis or how fast is
   * the camera rotating.
   */
  float angularSensibilityX;

  /**
   * Defines the pointer angular sensibility along the Y axis or how fast is the
   * camera rotating.
   */
  float angularSensibilityY;

  /**
   * Defines the pointer pinch precision or how fast is the camera zooming.
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
   * Defines the pointer panning sensibility or how fast is the camera moving.
   */
  float panningSensibility;

  /**
   * Defines whether panning (2 fingers swipe) is enabled through multitouch.
   */
  bool multiTouchPanning;

  /**
   * Defines whether panning is enabled for both pan (2 fingers swipe) and zoom
   * (pinch) through multitouch.
   */
  bool multiTouchPanAndZoom;

  /**
   * Revers pinch action direction.
   */
  bool pinchInwards;

private:
  bool _isPanClick;
  size_t _twoFingerActivityCount;
  bool _isPinching;

}; // end of class ArcRotateCameraPointersInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H
