#ifndef BABYLON_CAMERAS_INPUTS_FLY_CAMERA_MOUSE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FLY_CAMERA_MOUSE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/fly_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

/**
 * @brief Listen to mouse events to control the camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FlyCameraMouseInput : public ICameraInput<FlyCamera> {

public:
  /**
   * @brief Listen to mouse events to control the camera.
   * @param touchEnabled Define if touch is enabled. (Default is true.)
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   */
  FlyCameraMouseInput(bool touchEnabled = true);
  ~FlyCameraMouseInput() override; // = default

  /**
   * @brief Attach the mouse control to the HTML DOM element.
   * @param noPreventDefault Defines whether events caught by the controls should call
   * preventdefault().
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
   * @returns the class name.
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input's friendly name.
   */
  [[nodiscard]] std::string getSimpleName() const override;

protected:
  /**
   * @brief Track mouse movement, when the pointer is not locked.
   */
  void _pointerInput(PointerInfo* p, EventState& s);

  /**
   * @brief Track mouse movement, when pointer is locked.
   */
  void _onMouseMove(MouseEvent& e);

  /**
   * @brief Rotate camera by mouse offset.
   */
  void rotateCamera(float offsetX, float offsetY);

public:
  /**
   * Defines if touch is enabled. (Default is true.)
   */
  bool touchEnabled;

  /**
   * Defines the buttons associated with the input to handle camera move.
   */
  std::array<MouseButtonType, 3> buttons;

  /**
   * Assign buttons for Yaw control.
   */
  Int32Array buttonsYaw;

  /**
   * Assign buttons for Pitch control.
   */
  Int32Array buttonsPitch;

  /**
   * Assign buttons for Roll control.
   */
  Int32Array buttonsRoll;

  /**
   * Detect if any button is being pressed while mouse is moved.
   * -1 = Mouse locked.
   * 0 = Left button.
   * 1 = Middle Button.
   * 2 = Right Button.
   */
  int activeButton;

  /**
   * Defines the pointer's angular sensibility, to control the camera rotation
   * speed. Higher values reduce its sensitivity.
   */
  float angularSensibility;

private:
  Observer<PointerInfo>::Ptr _observer;
  Observer<Scene>::Ptr _rollObserver;
  std::optional<PositionCoord> previousPosition;
  std::optional<bool> _noPreventDefault;
  ICanvas* element;

}; // end of class FlyCameraMouseInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FLY_CAMERA_MOUSE_INPUT_H
