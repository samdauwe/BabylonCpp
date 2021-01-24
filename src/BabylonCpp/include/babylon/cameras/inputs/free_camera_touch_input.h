#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

/**
 * @brief Manage the touch inputs to control the movement of a free camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FreeCameraTouchInput : public ICameraInput<FreeCamera> {

public:
  /**
   * @brief Manage the touch inputs to control the movement of a free camera.
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   * @param allowMouse Defines if mouse events can be treated as touch events
   */
  FreeCameraTouchInput(bool allowMouse = false);
  ~FreeCameraTouchInput() override; // = default

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

public:
  /**
   * Define if mouse events can be treated as touch events
   */
  bool allowMouse;

  /**
   * Defines the touch sensibility for rotation.
   * The higher the faster.
   */
  float touchAngularSensibility;

  /**
   * Defines the touch sensibility for move.
   * The higher the faster.
   */
  float touchMoveSensibility;

private:
  std::optional<float> _offsetX;
  std::optional<float> _offsetY;

  Int32Array _pointerPressed;
  std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  Observer<PointerInfo>::Ptr _observer;
  std::function<void(FocusEvent& e)> _onLostFocus;

  std::optional<PositionCoord> previousPosition;
  bool noPreventDefault;

}; // end of class FreeCameraTouchInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H
