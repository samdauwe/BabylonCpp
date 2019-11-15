#ifndef BABYLON_CAMERAS_FLY_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_FLY_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class FlyCamera;

/**
 * @brief Default Inputs manager for the FlyCamera.
 * It groups all the default supported inputs for ease of use.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
struct BABYLON_SHARED_EXPORT FlyCameraInputsManager
    : public CameraInputsManager<FlyCamera> {

  /**
   * @brief Instantiates a new FlyCameraInputsManager.
   * @param camera Defines the camera the inputs belong to.
   */
  FlyCameraInputsManager(FlyCamera* camera);
  ~FlyCameraInputsManager(); // = default

  /**
   * @brief Add keyboard input support to the input manager.
   * @returns the new FlyCameraKeyboardMoveInput().
   */
  FlyCameraInputsManager& addKeyboard();

  /**
   * @brief Add mouse input support to the input manager.
   * @param touchEnabled Enable touch screen support.
   * @returns the new FlyCameraMouseInput().
   */
  FlyCameraInputsManager& addMouse(bool touchEnabled = true);

}; // end of struct FlyCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FLY_CAMERA_INPUTS_MANAGER_H
