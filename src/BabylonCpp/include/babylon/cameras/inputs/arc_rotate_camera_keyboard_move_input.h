#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class KeyboardInfo;

/**
 * @brief Manage the keyboard inputs to control the movement of an arc rotate camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraKeyboardMoveInput
    : public ICameraInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraKeyboardMoveInput();
  ~ArcRotateCameraKeyboardMoveInput() override; // = default

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

public:
  /**
   * Defines the list of key codes associated with the up action (increase alpha)
   */
  Int32Array keysUp;

  /**
   * Defines the list of key codes associated with the down action (decrease alpha)
   */
  Int32Array keysDown;

  /**
   * Defines the list of key codes associated with the left action (increase beta)
   */
  Int32Array keysLeft;

  /**
   * Defines the list of key codes associated with the right action (decrease beta)
   */
  Int32Array keysRight;

  /**
   * Defines the list of key codes associated with the reset action.
   * Those keys reset the camera to its last stored state (with the method camera.storeState())
   */
  Int32Array keysReset;

  /**
   * Defines the panning sensibility of the inputs.
   * (How fast is the camera panning)
   */
  float panningSensibility;

  /**
   * Defines the zooming sensibility of the inputs.
   * (How fast is the camera zooming)
   */
  float zoomingSensibility;

  /**
   * Defines whether maintaining the alt key down switch the movement mode from orientation to zoom.
   */
  bool useAltToZoom;

  /**
   * Rotation speed of the camera
   */
  float angularSpeed;

private:
  bool _noPreventDefault;
  Int32Array _keys;
  bool _ctrlPressed;
  bool _altPressed;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  Observer<KeyboardInfo>::Ptr _onKeyboardObserver;
  Engine* _engine;
  Scene* _scene;

}; // end of class ArcRotateCameraKeyboardMoveInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H
