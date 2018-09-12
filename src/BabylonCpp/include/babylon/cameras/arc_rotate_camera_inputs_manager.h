#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class ArcRotateCamera;

struct BABYLON_SHARED_EXPORT ArcRotateCameraInputsManager
    : public CameraInputsManager<ArcRotateCamera> {

  ArcRotateCameraInputsManager(ArcRotateCamera* camera);
  ~ArcRotateCameraInputsManager();

  ArcRotateCameraInputsManager& addMouseWheel();
  ArcRotateCameraInputsManager& addPointers();
  ArcRotateCameraInputsManager& addKeyboard();
  ArcRotateCameraInputsManager& addGamepad();

}; // end of struct ArcRotateCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
