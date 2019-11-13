#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class ArcRotateCamera;

/**
 * @brief Default Inputs manager for the ArcRotateCamera.
 * It groups all the default supported inputs for ease of use.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
struct BABYLON_SHARED_EXPORT ArcRotateCameraInputsManager
    : public CameraInputsManager<ArcRotateCamera> {

  /**
   * @brief Instantiates a new ArcRotateCameraInputsManager.
   * @param camera Defines the camera the inputs belong to
   */
  ArcRotateCameraInputsManager(ArcRotateCamera* camera);
  ~ArcRotateCameraInputsManager(); // = default

  /**
   * @brief Add mouse wheel input support to the input manager.
   * @returns the current input manager
   */
  ArcRotateCameraInputsManager& addMouseWheel();

  /**
   * @brief Add pointers input support to the input manager.
   * @returns the current input manager
   */
  ArcRotateCameraInputsManager& addPointers();

  /**
   * @brief Add keyboard input support to the input manager.
   * @returns the current input manager
   */
  ArcRotateCameraInputsManager& addKeyboard();

  /**
   * @brief Adds gamepad input support to the input manager.
   * @returns the current input manager
   */
  ArcRotateCameraInputsManager& addGamepad();

}; // end of struct ArcRotateCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
