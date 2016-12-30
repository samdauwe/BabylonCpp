#ifndef BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT FreeCameraInputsManager
  : public CameraInputsManager<FreeCamera> {

  FreeCameraInputsManager(FreeCamera* _camera);
  ~FreeCameraInputsManager();

  FreeCameraInputsManager& addKeyboard();
  FreeCameraInputsManager& addMouse(bool touchEnabled = true);

}; // end of struct FreeCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
