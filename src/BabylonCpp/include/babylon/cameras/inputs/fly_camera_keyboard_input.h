#ifndef BABYLON_CAMERAS_INPUTS_FLY_CAMERA_KEYBOARD_MOVE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FLY_CAMERA_KEYBOARD_MOVE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/fly_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class KeyboardInfo;

/**
 * @brief Listen to keyboard events to control the camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FlyCameraKeyboardInput : public ICameraInput<FlyCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  FlyCameraKeyboardInput();
  ~FlyCameraKeyboardInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should call
   * preventdefault() (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Update the current camera state depending on the inputs that have been used this frame.
   * This is a dynamically created lambda to avoid the performance penalty of looping for inputs in
   * the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Hidden
   */
  void _onLostFocus(const FocusEvent& e);

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
   * The list of keyboard keys used to control the forward move of the camera.
   */
  Int32Array keysForward;

  /**
   * The list of keyboard keys used to control the backward move of the camera.
   */
  Int32Array keysBackward;

  /**
   * The list of keyboard keys used to control the forward move of the camera.
   */
  Int32Array keysUp;

  /**
   * The list of keyboard keys used to control the backward move of the camera.
   */
  Int32Array keysDown;

  /**
   * The list of keyboard keys used to control the right strafe move of the
   * camera.
   */
  Int32Array keysRight;

  /**
   * The list of keyboard keys used to control the left strafe move of the
   * camera.
   */
  Int32Array keysLeft;

private:
  ICanvas* _canvas;
  bool _noPreventDefault;
  Int32Array _keys;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  Observer<KeyboardInfo>::Ptr _onKeyboardObserver;
  Engine* _engine;
  Scene* _scene;

}; // end of class FlyCameraKeyboardInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FLY_CAMERA_KEYBOARD_MOVE_INPUT_H
