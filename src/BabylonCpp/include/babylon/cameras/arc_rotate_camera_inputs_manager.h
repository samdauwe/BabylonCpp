#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class ArcRotateCamera;

/**
 * @brief Interface representing an arc rotate camera inputs manager.
 */
struct BABYLON_SHARED_EXPORT ArcRotateCameraInputsManager
    : public CameraInputsManager<ArcRotateCamera> {

  ArcRotateCameraInputsManager(ArcRotateCamera* camera);
  ~ArcRotateCameraInputsManager();

  ArcRotateCameraInputsManager& addMouseWheel();
  ArcRotateCameraInputsManager& addPointers();
  ArcRotateCameraInputsManager& addKeyboard();

  /**
   * @brief Adds gamepad input support to the ArcRotateCamera InputManager.
   * @returns the camera inputs manager
   */
  ArcRotateCameraInputsManager& addGamepad();

}; // end of struct ArcRotateCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
