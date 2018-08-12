#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FreeCameraTouchInput
    : public ICameraInput<FreeCamera> {

public:
  FreeCameraTouchInput();
  virtual ~FreeCameraTouchInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  float touchAngularSensibility;
  float touchMoveSensibility;

private:
  nullable_t<float> _offsetX;
  nullable_t<float> _offsetY;

  Int32Array _pointerPressed;
  ::std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  Observer<PointerInfo>::Ptr _observer;
  ::std::function<string_t(FocusEvent& e)> _onLostFocus;

}; // end of class FreeCameraTouchInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_TOUCH_INPUT_H
