#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_GAMEPAD_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_GAMEPAD_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class Gamepad;

/**
 * @brief Manage the gamepad inputs to control an arc rotate camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraGamepadInput : public ICameraInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraGamepadInput();
  ~ArcRotateCameraGamepadInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault) override;

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
  std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  std::string getSimpleName() const override;

public:
  /**
   * Defines the gamepad the input is gathering event from.
   */
  Gamepad* gamepad;

  /**
   * Defines the gamepad rotation sensiblity.
   * This is the threshold from when rotation starts to be accounted for to
   * prevent jittering.
   */
  float gamepadRotationSensibility;

  /**
   * Defines the gamepad move sensiblity.
   * This is the threshold from when moving starts to be accounted for for to
   * prevent jittering.
   */
  float gamepadMoveSensibility;

private:
  Observer<Gamepad>::Ptr _onGamepadConnectedObserver;
  Observer<Gamepad>::Ptr _onGamepadDisconnectedObserver;

}; // end of class ArcRotateCameraGamepadInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_GAMEPAD_INPUT_H
