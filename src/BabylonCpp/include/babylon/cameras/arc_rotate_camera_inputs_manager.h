#ifndef BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ArcRotateCameraInputsManager
  : public CameraInputsManager<ArcRotateCamera> {

  ArcRotateCameraInputsManager(ArcRotateCamera* camera);
  ~ArcRotateCameraInputsManager();

  ArcRotateCameraInputsManager& addMouseWheel();
  ArcRotateCameraInputsManager& addPointers();
  ArcRotateCameraInputsManager& addKeyboard();
}; // end of struct ArcRotateCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_ROTATE_CAMERA_INPUTS_MANAGER_H
