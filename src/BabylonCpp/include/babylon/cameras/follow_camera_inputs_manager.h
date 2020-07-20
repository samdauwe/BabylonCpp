#ifndef BABYLON_CAMERAS_FOLLOW_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_FOLLOW_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class FollowCamera;

/**
 * @brief Default Inputs manager for the FollowCamera.
 * It groups all the default supported inputs for ease of use.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
struct BABYLON_SHARED_EXPORT FollowCameraInputsManager : public CameraInputsManager<FollowCamera> {

  /**
   * @brief Instantiates a new FollowCameraInputsManager.
   * @param camera Defines the camera the inputs belong to
   */
  FollowCameraInputsManager(FollowCamera* camera);
  ~FollowCameraInputsManager(); // = default

  /**
   * @brief Add keyboard input support to the input manager.
   * @returns the current input manager
   */
  FollowCameraInputsManager& addKeyboard();

  /**
   * @brief Add mouse wheel input support to the input manager.
   * @returns the current input manager
   */
  FollowCameraInputsManager& addMouseWheel();

  /**
   * @brief Add pointers input support to the input manager.
   * @returns the current input manager
   */
  FollowCameraInputsManager& addPointers();

  /**
   * @brief Add orientation input support to the input manager.
   * @returns the current input manager
   */
  FollowCameraInputsManager& addVRDeviceOrientation();

}; // end of struct FollowCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FOLLOW_INPUTS_MANAGER_H
