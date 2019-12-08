#ifndef BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class FreeCamera;
class FreeCameraMouseInput;
using FreeCameraMouseInputPtr = std::shared_ptr<FreeCameraMouseInput>;

/**
 * @brief Default Inputs manager for the FreeCamera.
 * It groups all the default supported inputs for ease of use.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
struct BABYLON_SHARED_EXPORT FreeCameraInputsManager
    : public CameraInputsManager<FreeCamera> {

  /**
   * @brief Instantiates a new FreeCameraInputsManager.
   * @param camera Defines the camera the inputs belong to
   */
  FreeCameraInputsManager(FreeCamera* iCamera);
  ~FreeCameraInputsManager(); // = default

  /**
   * @brief Add keyboard input support to the input manager.
   * @returns the current input manager
   */
  FreeCameraInputsManager& addKeyboard();

  /**
   * @brief Add mouse input support to the input manager.
   * @param touchEnabled if the FreeCameraMouseInput should support touch
   * (default: true)
   * @returns the current input manager
   */
  FreeCameraInputsManager& addMouse(bool touchEnabled = true);

  /**
   * @brief Removes the mouse input support from the manager
   * @returns the current input manager
   */
  FreeCameraInputsManager& removeMouse();

  /**
-   * @brief Add orientation input support to the input manager.
   * @returns the current input manager
   */
  FreeCameraInputsManager& addDeviceOrientation();

  /**
   * @brief Adds gamepad input support to the input manager.
   * @returns the current input manager
   */
  FreeCameraInputsManager& addGamepad();

  /**
   * @brief Add touch input support to the input manager.
   * @returns the current input manager
   */
  FreeCameraInputsManager& addTouch();

  /**
   * @brief Add virtual joystick input support to the input manager.
   * @returns the current input manager
   */
  FreeCameraInputsManager& addVirtualJoystick();

  /**
   * @brief Remove all attached input methods from a camera.
   */
  void clear();

  /**
   * Hidden
   */
  FreeCameraMouseInputPtr _mouseInput;

}; // end of struct FreeCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
