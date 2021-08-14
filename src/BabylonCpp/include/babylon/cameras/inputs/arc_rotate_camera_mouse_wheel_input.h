#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/maths/plane.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

struct IWheelEvent;

/**
 * @brief Manage the mouse wheel inputs to control an arc rotate camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraMouseWheelInput : public ICameraInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraMouseWheelInput();
  ~ArcRotateCameraMouseWheelInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input from.
   * @param noPreventDefault Defines whether event caught by the controls should call
   * preventdefault() (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param ignored defines an ignored parameter kept for backward compatibility. If you want to
   * define the source input element, you can set engine.inputElement before calling
   * camera.attachControl
   */
  void detachControl(ICanvas* ignored = nullptr) override;

  /**
   * @brief Update the current camera state depending on the inputs that have been used this frame.
   * This is a dynamically created lambda to avoid the performance penalty of looping for inputs in
   * the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  [[nodiscard]] std::string getSimpleName() const override;

protected:
  float computeDeltaFromMouseWheelLegacyEvent(float mouseWheelDelta, float radius);
  void _updateHitPlane();
  // Get position on the hit plane
  Vector3 _getPosition() const;
  void _zoomToMouse(float delta);
  // Sets x y or z of passed in vector to zero if less than Epsilon.
  void _zeroIfClose(Vector3& vec);

public:
  /**
   * Firefox uses a different scheme to report scroll distances to other
   * browsers. Rather than use complicated methods to calculate the exact
   * multiple we need to apply, let's just cheat and use a constant.
   * https://developer.mozilla.org/en-US/docs/Web/API/WheelEvent/deltaMode
   * https://stackoverflow.com/questions/20110224/what-is-the-height-of-a-line-in-a-wheel-event-deltamode-dom-delta-line
   */
  static const float ffMultiplier;

  /**
   * Gets or Set the mouse wheel precision or how fast is the camera zooming.
   */
  float wheelPrecision;

  /**
   * Gets or Set the boolean value that controls whether or not the mouse wheel
   * zooms to the location of the mouse pointer or not.  The default is false.
   */
  bool zoomToMouseLocation;

  /**
   * wheelDeltaPercentage will be used instead of wheelPrecision if different from 0.
   * It defines the percentage of current camera.radius to use as delta when wheel is used.
   */
  float wheelDeltaPercentage;

  /**
   * If set, this function will be used to set the radius delta that will be added to the current
   * camera radius
   */
  std::function<float(float wheelDelta, ArcRotateCameraMouseWheelInput* input,
                      const IWheelEvent& event)>
    customComputeDeltaFromMouseWheel;

private:
  bool _noPreventDefault;
  std::function<void(PointerInfo* p, EventState& es)> _wheel;
  Observer<PointerInfo>::Ptr _observer;
  std::optional<Plane> _hitPlane;
  Vector3 _inertialPanning;

}; // end of class ArcRotateCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
