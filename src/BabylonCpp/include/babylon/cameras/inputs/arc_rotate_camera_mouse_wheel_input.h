#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

/**
 * @brief Manage the mouse wheel inputs to control an arc rotate camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraMouseWheelInput
    : public ICameraInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraMouseWheelInput();
  virtual ~ArcRotateCameraMouseWheelInput(); // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Update the current camera state depending on the inputs that have
   * been used this frame. This is a dynamically created lambda to avoid the
   * performance penalty of looping for inputs in the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current intput.
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  const std::string getSimpleName() const override;

private:
  float computeDeltaFromMouseWheelLegacyEvent(
    const MouseWheelEvent& mouseWheelLegacyEvent, float radius) const;

public:
  /**
   * Gets or Set the mouse wheel precision or how fast is the camera zooming.
   */
  float wheelPrecision;

  /**
   * wheelDeltaPercentage will be used instead of wheelPrecision if different
   * from 0.
   * It defines the percentage of current camera.radius to use as delta when
   * wheel is used.
   */
  float wheelDeltaPercentage;

private:
  ICanvas* _canvas;
  bool _noPreventDefault;
  std::function<void(PointerInfo* p, EventState& es)> _wheel;
  Observer<PointerInfo>::Ptr _observer;

}; // end of class ArcRotateCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
