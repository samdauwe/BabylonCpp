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
   * @brief Gets the class name of the current intput.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  [[nodiscard]] std::string getSimpleName() const override;

private:
  [[nodiscard]] float computeDeltaFromMouseWheelLegacyEvent(float mouseWheelDelta,
                                                            float radius) const;

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
  bool _noPreventDefault;
  std::function<void(PointerInfo* p, EventState& es)> _wheel;
  Observer<PointerInfo>::Ptr _observer;

}; // end of class ArcRotateCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
