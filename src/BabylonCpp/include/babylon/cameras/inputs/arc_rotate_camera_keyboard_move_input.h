#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ArcRotateCameraKeyboardMoveInput
    : public ICameraInput<ArcRotateCamera> {

public:
  ArcRotateCameraKeyboardMoveInput();
  virtual ~ArcRotateCameraKeyboardMoveInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  Int32Array keysUp;
  Int32Array keysDown;
  Int32Array keysLeft;
  Int32Array keysRight;

private:
  ICanvas* _canvas;
  bool _noPreventDefault;
  Int32Array _keys;
  ::std::function<void(const KeyboardEvent& e)> _onKeyDown;
  ::std::function<void(const KeyboardEvent& e)> _onKeyUp;
  ::std::function<void(const FocusEvent& e)> _onLostFocus;

}; // end of class ArcRotateCameraKeyboardMoveInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H
