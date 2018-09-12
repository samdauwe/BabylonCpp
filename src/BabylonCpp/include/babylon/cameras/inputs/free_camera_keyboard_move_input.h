#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_KEYBOARD_MOVE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_KEYBOARD_MOVE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class KeyboardInfo;

class BABYLON_SHARED_EXPORT FreeCameraKeyboardMoveInput
    : public ICameraInput<FreeCamera> {

public:
  FreeCameraKeyboardMoveInput();
  virtual ~FreeCameraKeyboardMoveInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;
  /** Hidden */
  void _onLostFocus(const FocusEvent& e);

public:
  Int32Array keysUp;
  Int32Array keysDown;
  Int32Array keysLeft;
  Int32Array keysRight;

private:
  Int32Array _keys;
  ICanvas* _canvas;
  bool _noPreventDefault;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  Observer<KeyboardInfo>::Ptr _onKeyboardObserver;
  Engine* _engine;
  Scene* _scene;

}; // end of class FreeCameraKeyboardMoveInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_KEYBOARD_MOVE_INPUT_H
