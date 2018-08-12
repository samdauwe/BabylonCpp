#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_GAMEPAD_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_GAMEPAD_INPUT_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FreeCameraGamepadInput
    : public ICameraInput<FreeCamera> {
public:
  FreeCameraGamepadInput();
  virtual ~FreeCameraGamepadInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  Gamepad* gamepad;

  float gamepadAngularSensibility;
  float gamepadMoveSensibility;

private:
  Observer<Gamepad>::Ptr _onGamepadConnectedObserver;
  Observer<Gamepad>::Ptr _onGamepadDisconnectedObserver;

  Matrix _cameraTransform;
  Vector3 _deltaTransform;
  Vector3 _vector3;
  Vector2 _vector2;

}; // end of class FreeCameraGamepadInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_GAMEPAD_INPUT_H
