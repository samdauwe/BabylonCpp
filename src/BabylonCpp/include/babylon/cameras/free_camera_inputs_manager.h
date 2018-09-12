#ifndef BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera_inputs_manager.h>

namespace BABYLON {

class FreeCamera;

struct BABYLON_SHARED_EXPORT FreeCameraInputsManager
    : public CameraInputsManager<FreeCamera> {

  FreeCameraInputsManager(FreeCamera* _camera);
  ~FreeCameraInputsManager();

  FreeCameraInputsManager& addKeyboard();
  FreeCameraInputsManager& addMouse(bool touchEnabled = true);
  FreeCameraInputsManager& addGamepad();
  FreeCameraInputsManager& addDeviceOrientation();
  FreeCameraInputsManager& addTouch();
  FreeCameraInputsManager& addVirtualJoystick();

}; // end of struct FreeCameraInputsManager

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_INPUTS_MANAGER_H
